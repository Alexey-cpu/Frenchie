#include <FrenchieApplicationLayerRenderingQueue.hpp>

using namespace Frenchie::Application;

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>
#include <FrenchieApplicationPlatformBackend.hpp>

// STL
#include <algorithm>

// RenderingQueue
RenderingQueue::RenderingQueue(const std::string& _Name) : Layer(_Name){}
RenderingQueue::~RenderingQueue(){}

RenderingQueueMetrics RenderingQueue::get_rendering_queue_metrics() const
{
    return m_Metrics;
}

float RenderingQueue::get_minimum_line_width() const
{
    return m_MeshLineMinimumWidth;
}

gs_vec3f RenderingQueue::get_cursor_postion() const
{
    gs_vec2f size   = ApplicationPlatformBackend::get_window_size();
    gs_vec2f cursor = ApplicationPlatformBackend::get_window_cursor_position();
    gs_mat4f matrix =
        gs_matrix_invert_square(m_CameraViewMatrix) *
        gs_matrix_invert_square(m_ProjectionMatrix);

    return matrix * gs_vec4f(ApplicationRenderingBackend::convert_to_NDC(cursor, size), -1.f, 1.f);
}

float RenderingQueue::get_near_plane() const
{
    return -10000.f;
}

float RenderingQueue::get_far_plane() const
{
    return +10000.f;
}

bool RenderingQueue::awake()
{
    return true;
}

void RenderingQueue::frame_start()
{
    // assetion
    GS_ASSERT(m_MeshVertexes.empty());
    GS_ASSERT(m_MeshVertexesIndexes.empty());
    GS_ASSERT(m_MeshVertexesIndexesOffset == 0);

    // clean-up
    if(!m_MeshDataWantsCleanUp)
    {
        m_MeshDataWantsCleanUp     = true;
        m_MeshDataCleanUpTimePoint = Frenchie::Core::Clock::tic();
    }

    // metrics
    m_FrameRateMeasurementStartTimePoint = Frenchie::Core::Clock::tic();

    // push clear color
    push_clear_color(current_clear_color());
    push_clip_box(current_clipping_box());

    // compute projection matrix
    float width  = ApplicationPlatformBackend::get_window_size().x;
    float height = ApplicationPlatformBackend::get_window_size().y;

    auto camera = ApplicationRenderingBackend::calculate_2d_camera_view_and_projection(
        gs_vec2f(width * 0.5f, height * 0.5f),
        gs_vec2f(width, height),
        0.f,
        get_near_plane(),
        get_far_plane());

    m_CameraViewMatrix = camera.CameraView;
    m_ProjectionMatrix = camera.Projection;

    // compute viewport
    gs_vec3f viewportMin = ApplicationRenderingBackend::convert_to_NDC(gs_vec2f(0.f, 0.f), gs_vec2f(width, height));
    gs_vec3f viewportMax = ApplicationRenderingBackend::convert_to_NDC(ApplicationPlatformBackend::get_window_size(), gs_vec2f(width, height));

    m_Viewport = gs_2d_boxf(
        gs_matrix_invert_square(m_ProjectionMatrix) * gs_matrix_invert_square(m_CameraViewMatrix) * gs_vec4f(viewportMin, 1.f),
        gs_matrix_invert_square(m_ProjectionMatrix) * gs_matrix_invert_square(m_CameraViewMatrix) * gs_vec4f(viewportMax, 1.f));
}

void RenderingQueue::frame_update()
{
}

void RenderingQueue::frame_render()
{
    if(m_MeshVertexes.empty() || m_MeshVertexesIndexes.empty()) return;

    if(!ApplicationRenderingBackend::begin_render(
        &m_MeshVertexes[0],
        (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size(),
        &m_MeshVertexesIndexes[0],
        (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexesIndexes.size()))
    {
        return;
    }

    // sort rendering commands by depth
    std::stable_sort(
        m_Commands.begin(),
        m_Commands.end(),
        [this](const RenderingQueueCommand& _A, const RenderingQueueCommand& _B)
        {
            auto retrieve_depth_coordinate = [](const RenderingQueueCommand& _Command)->float
            {
                return _Command.Command.has_value() ? gs_matrix_retrieve_transform_translation_vector(_Command.Command.value().Transform)[2] : 0.f;
            };

            return Frenchie::Application::ApplicationRenderingBackend::compare_objects_depths(retrieve_depth_coordinate(_A), retrieve_depth_coordinate(_B));
        }
    );

    // compute projection matrix
    gs_mat4f projectionMatrix = m_ProjectionMatrix * m_CameraViewMatrix;

    // execute rendering commands
    for (int i = 0; i < (int)m_Commands.size(); ++i)
    {
        // clipping box
        auto rendererCommandClippingBox  = m_Commands[i].ClippingBox;

        if(rendererCommandClippingBox.has_value())
            ApplicationRenderingBackend::scissor_box(rendererCommandClippingBox.value().ClippingBox);

        // clear color
        auto rendererCommandClearColor  = m_Commands[i].ClearColor;

        if(rendererCommandClearColor.has_value())
            ApplicationRenderingBackend::clear_color(rendererCommandClearColor.value().ClearColor);

        // mesh rendering hints
        auto rendererCommandMeshRenderingHints = m_Commands[i].MeshRenderingHints;

        if(rendererCommandMeshRenderingHints.has_value())
            ApplicationRenderingBackend::mesh_rendering_hints(m_Commands[i].MeshRenderingHints.value().Hints);

        // mesh rendering command
        auto renderingCommand = m_Commands[i].Command;

        if(renderingCommand.has_value())
        {
            auto mesh               = renderingCommand.value().Mesh;
            auto texture            = renderingCommand.value().Texture;
            auto transformMatrix    = renderingCommand.value().Transform;

            ApplicationRenderingBackend::render_mesh(
                mesh.SourceMeshVertex,
                mesh.TargetMeshVertex,
                texture,
                projectionMatrix * transformMatrix);
        }
    }

    // save metrics
    m_Metrics.RenderingCommandsCount = (int)m_Commands.size();
    m_Metrics.RenderedTrianglesCount = (int)(m_MeshVertexes.size() / 3);
    double current = (double)1e9 / Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Nanoseconds>(m_FrameRateMeasurementStartTimePoint, Frenchie::Core::Clock::tic());
    m_FrameRateMeasurementFilterBuffer.push(current);
    m_Metrics.FrameRate += (current - m_FrameRateMeasurementFilterBuffer.at(m_FrameRateMeasurementFilterBuffer.size() - 1)) / (double)(m_FrameRateMeasurementFilterBuffer.size());

    // clear commands queue
    m_Commands.clear();
}

void RenderingQueue::frame_finish()
{
    // clear rendering commands data
    m_ClearColors.clear();
    m_ClippingBoxes.clear();
    m_MeshRenderingHints.clear();
    m_TesselationTolerance.clear();

    // clear meshes
    m_MeshVertexes.clear();
    m_MeshVertexesIndexes.clear();

    // restore mesh offsets
    m_MeshVertexesIndexesOffset = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();

    if(m_MeshDataWantsCleanUp &&
        Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Seconds>(m_MeshDataCleanUpTimePoint, Frenchie::Core::Clock::tic()) > m_MeshDataCleanUpInterval)
    {
        m_MeshDataWantsCleanUp = false;
        std::vector<ApplicationRenderingBackendMeshVertex>(m_MeshVertexes).swap(m_MeshVertexes);
        std::vector<ApplicationRenderingBackendMeshVertexIndex>(m_MeshVertexesIndexes).swap(m_MeshVertexesIndexes);
    }
}

void RenderingQueue::finish()
{
}

void RenderingQueue::quit(){}

bool RenderingQueue::allows_multiple_instances() const
{
    return true;
}

void RenderingQueue::push_rendering_command(const gs_mat4f& _Transform)
{
    push_rendering_command(ApplicationRenderingBackend::get_default_texture(), gs_color_rgba(255, 255, 255, 255), _Transform);
}

void RenderingQueue::push_rendering_command(const ApplicationRenderingBackendTexture& _Texture, const gs_color& _Color, const gs_mat4f& _Transform)
{
    if(m_MeshVertexesIndexes.empty() || m_MeshVertexes.empty()) return;

    m_Commands.push_back(
        RenderingQueueCommand(

            // mesh rendering command
            RenderingQueueRenderingCommand(
                RenderingQueueMesh(
                    m_MeshVertexesIndexesOffset,
                    (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexesIndexes.size()),

                ApplicationRenderingBackendTexture(
                    _Texture.Ptr,
                    _Texture.Width,
                    _Texture.Height,
                    _Color,
                    _Texture.Format,
                    _Texture.Wrap,
                    _Texture.MinFilter,
                    _Texture.MaxFilter),

                _Transform),

            // clear color
            RenderingQueueRendererCommandClearColor(current_clear_color()),

            // clipping box
            RenderingQueueRendererCommandClippingBox(current_clipping_box()),

            // mesh rendering hints
            RenderingQueueRendererCommandMeshRenderingHints(current_mesh_rendering_hints())));

    // move offsets
    m_MeshVertexesIndexesOffset  = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();
}

void RenderingQueue::push_clip_box(const gs_2d_boxf& _Value, const gs_mat4f& _Transform)
{
    gs_2d_boxf clipRect = _Value.transform(_Transform);
    m_ClippingBoxes.push_back(clipRect);
}

void RenderingQueue::pop_clip_box()
{
    if(!m_ClippingBoxes.empty())
        m_ClippingBoxes.pop_back();
}

void RenderingQueue::push_clear_color(const gs_color& _Value)
{
    m_ClearColors.push_back(_Value);
}

void RenderingQueue::pop_clear_color()
{
    if(!m_ClearColors.empty())
        m_ClearColors.pop_back();
}

void RenderingQueue::push_mesh_rendering_hints(const ApplicationRenderingBackendMeshRenderingHints& _Hints)
{
    m_MeshRenderingHints.push_back(_Hints);
}

void RenderingQueue::pop_mesh_rendering_hints()
{
    if(!m_MeshRenderingHints.empty())
        m_MeshRenderingHints.pop_back();
}

void RenderingQueue::push_tesselation_tolerance(const float& _Value)
{
    m_TesselationTolerance.push_back(_Value);
}

void RenderingQueue::pop_tesselation_tolerance()
{
    if (!m_TesselationTolerance.empty())
        m_TesselationTolerance.pop_back();
}

gs_2d_boxf RenderingQueue::current_clipping_box() const
{
    return !m_ClippingBoxes.empty() ?
                m_ClippingBoxes[m_ClippingBoxes.size() - 1] :
                    gs_2d_boxf(gs_vec2f(0.f, 0.f), ApplicationPlatformBackend::get_window_size());
}

gs_2d_boxf RenderingQueue::current_viewport() const
{
    return m_Viewport;
}

gs_color RenderingQueue::current_clear_color() const
{
    return !m_ClearColors.empty() ?
                m_ClearColors[m_ClearColors.size() - 1] :
                    gs_color_rgba(255, 255, 255, 255);
}

ApplicationRenderingBackendMeshRenderingHints RenderingQueue::current_mesh_rendering_hints() const
{
    return !m_MeshRenderingHints.empty() ?
                m_MeshRenderingHints[m_MeshRenderingHints.size() - 1] :
                    ApplicationRenderingBackendMeshRenderingHints_::ApplicationRenderingBackendMeshRenderingHints_Triangles;
}

float RenderingQueue::current_tesselation_tolerance() const
{
    return !m_TesselationTolerance.empty() ?
                m_TesselationTolerance[m_TesselationTolerance.size() - 1] :
                    0.1f;
}

void RenderingQueue::begin_mesh()
{
    m_MeshVertexesStartingIndex = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();
}

void RenderingQueue::end_mesh()
{
    GS_ASSERT(m_MeshVertexesStartingIndex.has_value());
    for (ApplicationRenderingBackendMeshVertexIndex i = m_MeshVertexesStartingIndex.value(); i < (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size(); ++i)
        m_MeshVertexesIndexes.push_back(i);
    m_MeshVertexesStartingIndex.reset();
}