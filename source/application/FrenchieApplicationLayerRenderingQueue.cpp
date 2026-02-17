#include <FrenchieApplicationLayerRenderingQueue.hpp>

using namespace Frenchie::Application;

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>
#include <FrenchieApplicationPlatformBackend.hpp>

// STL
#include <algorithm>

RenderingQueue::RenderingQueue(){}
RenderingQueue::~RenderingQueue(){}

gs_mat4f RenderingQueue::get_projection_matrix() const
{
    return m_ProjectionMatrix;
}

gs_mat4f RenderingQueue::get_cameraview_matrix() const
{
    return m_CameraViewMatrix;
}

RenderingQueueMetrics RenderingQueue::get_rendering_queue_metrics() const
{
    return m_Metrics;
}

void RenderingQueue::set_projection_matrix(const gs_mat4f& _Matrix)
{
    m_ProjectionMatrix = _Matrix;
}

void RenderingQueue::set_cameraview_matrix(const gs_mat4f& _Matrix)
{
    m_CameraViewMatrix = _Matrix;
}

bool RenderingQueue::awake()
{
    return true;
}

void RenderingQueue::frame_start()
{
    m_FrameRateMeasurementStartTimePoint = Frenchie::Core::tic();
}

void RenderingQueue::frame_update()
{
}

void RenderingQueue::frame_render()
{
    ApplicationRenderingBackend::enable(
        ApplicationRenderingBackendGraphicsApiHints_::ApplicationRenderingBackendGraphicsApiHints_Blending    |
        ApplicationRenderingBackendGraphicsApiHints_::ApplicationRenderingBackendGraphicsApiHints_DepthTest   |
        ApplicationRenderingBackendGraphicsApiHints_::ApplicationRenderingBackendGraphicsApiHints_StencilTest |
        ApplicationRenderingBackendGraphicsApiHints_::ApplicationRenderingBackendGraphicsApiHints_ScissorTest);

    ApplicationRenderingBackend::clear_buffers(
        ApplicationRenderingBackendGraphicsApiBuffers_::ApplicationRenderingBackendGraphicsApiBuffers_Color |
        ApplicationRenderingBackendGraphicsApiBuffers_::ApplicationRenderingBackendGraphicsApiBuffers_Depth |
        ApplicationRenderingBackendGraphicsApiBuffers_::ApplicationRenderingBackendGraphicsApiBuffers_Stencil);

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

            return retrieve_depth_coordinate(_A) < retrieve_depth_coordinate(_B);
        }
    );

    // execute rendering commands
    gs_mat4f projectionMatrix = m_ProjectionMatrix * m_CameraViewMatrix;

    for (int i = 0; i < (int)m_Commands.size(); ++i)
    {
        // clipping box
        auto rendererCommandClippingBox  = m_Commands[i].ClippingBox;

        if(rendererCommandClippingBox.has_value())
        {
            // retrieve clipping rect and display scale
            auto clippingRect = rendererCommandClippingBox.value().ClippingBox;
            auto displayScale = ApplicationPlatformBackend::get_window_framebuffer_size() / ApplicationPlatformBackend::get_window_size();

            // apply clipping rect
            ApplicationRenderingBackend::scissor_box(
                gs_2dboxf(
                    clippingRect.Min * displayScale,
                    clippingRect.Max * displayScale));
        }

        // clear color
        auto rendererCommandClearColor  = m_Commands[i].ClearColor;

        if(rendererCommandClearColor.has_value())
            ApplicationRenderingBackend::clear_color(rendererCommandClearColor.value().ClearColor);

        // execute rendering command
        auto renderingCommand = m_Commands[i].Command;

        if(renderingCommand.has_value())
        {
            auto mesh               = renderingCommand.value().Mesh;
            auto shader             = renderingCommand.value().Shader;
            auto color              = renderingCommand.value().Texture.Color;
            auto texture            = renderingCommand.value().Texture;
            auto transformMatrix    = renderingCommand.value().Transform;
            auto meshRenderingHints = renderingCommand.value().MeshRendererHints;

            ApplicationRenderingBackend::begin_use_texture(texture);
            ApplicationRenderingBackend::begin_use_shader(shader);

            ApplicationRenderingBackend::set_shader_uniform(shader, "u_ModelMatrix", projectionMatrix * transformMatrix);
            ApplicationRenderingBackend::set_shader_uniform(shader, "u_Texture", 0);

            ApplicationRenderingBackend::begin_use_mesh(mesh, meshRenderingHints);
            
            ApplicationRenderingBackend::end_use_texture();
            ApplicationRenderingBackend::end_use_shader();
            ApplicationRenderingBackend::end_use_mesh();

            ApplicationRenderingBackend::destroy_mesh(mesh);
        }
    }

    // save metrics
    m_Metrics.RenderingCommandsCount = (int)m_Commands.size();

    // clear commands queue
    m_Commands.clear();

    // clean-up
    ApplicationRenderingBackend::disable(
        ApplicationRenderingBackendGraphicsApiHints_::ApplicationRenderingBackendGraphicsApiHints_Blending    |
        ApplicationRenderingBackendGraphicsApiHints_::ApplicationRenderingBackendGraphicsApiHints_DepthTest   |
        ApplicationRenderingBackendGraphicsApiHints_::ApplicationRenderingBackendGraphicsApiHints_StencilTest |
        ApplicationRenderingBackendGraphicsApiHints_::ApplicationRenderingBackendGraphicsApiHints_ScissorTest);
}

void RenderingQueue::frame_finish()
{
    // calculate frame rate and save metrics
    double current = (double)1e9 / Frenchie::Core::elapsed<std::chrono::nanoseconds>(m_FrameRateMeasurementStartTimePoint, Frenchie::Core::tic());
    m_FrameRateMeasurementFilterBuffer.push(current);
    m_Metrics.FrameRate              += (current - m_FrameRateMeasurementFilterBuffer.at(m_FrameRateMeasurementFilterBuffer.size() - 1)) / (double)(m_FrameRateMeasurementFilterBuffer.size());
    m_Metrics.RenderedTrianglesCount = m_RenderedTrianglesCount;
    m_RenderedTrianglesCount = 0;
}

void RenderingQueue::finish()
{
}

void RenderingQueue::quit(){}

bool RenderingQueue::allows_multiple_instances() const
{
    return true;
}

void RenderingQueue::push_rendering_command(
    const ApplicationRenderingBackendMesh&                      _Mesh,
    const ApplicationRenderingBackendShader&                    _Shader,
    const ApplicationRenderingBackendTexture&                   _Texture,
    const gs_mat4f&                                             _Transform,
    const ApplicationRenderingBackendGraphicsApiRenderingHints& _RendererHints,
    const ApplicationRenderingBackendColor&                     _ClearColor,
    const gs_2dboxf&                                            _ClippinBox)
{
    m_Commands.push_back(
        RenderingQueueCommand(
            RenderingQueueRenderingCommand(
                _Mesh,
                _Shader,
                _Texture,
                _Transform,
                _RendererHints),
        RenderingQueueRendererCommandClearColor(_ClearColor),
        RenderingQueueRendererCommandClippingBox(_ClippinBox)));
}