#include <FrenchieApplicationLayerRenderingQueue.hpp>

using namespace Frenchie::Application;

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>
#include <FrenchieApplicationPlatformBackend.hpp>

// STL
#include <algorithm>

RenderingQueuePathSegment::RenderingQueuePathSegment(const gs_vec2f& _P1, const gs_vec2f& _P2, const float& _Width)
{
    setup(_P1, _P2, _Width);
}

void RenderingQueuePathSegment::setup(const gs_vec2f& _P1, const gs_vec2f& _P2, const float& _Width)
{
    P1 = _P1;
    P2 = _P2;

    gs_vec3f direction     = gs_vector_normalize(_P2 - _P1);
    gs_vec2f perpendicular = gs_vector_normalize(gs_vector_cross(direction, gs_vec3f(0.f, 0.f, 1.f))) * _Width * 0.5f;
    
    P1min = _P1 - perpendicular;
    P1max = _P1 + perpendicular;
    P2min = _P2 - perpendicular;
    P2max = _P2 + perpendicular;
}

void RenderingQueuePathSegment::setup(const float& _Width)
{
    setup(P1, P2, _Width);
}

// RenderingQueuePathBuilder
RenderingQueuePathBuilder::RenderingQueuePathBuilder(const float& _PolygonLinesWidth) : m_PolygonLinesWidth(_PolygonLinesWidth){}
RenderingQueuePathBuilder::~RenderingQueuePathBuilder(){}

void RenderingQueuePathBuilder::begin(const gs_vec2f& _Point)
{
    m_PolygonLinesSourcePoint = _Point;
    m_PolygonLines.clear();
    m_PolygonLinesIndexes.clear();
}

void RenderingQueuePathBuilder::line_to(const gs_vec2f& _Target)
{
    m_PolygonLines.push_back(RenderingQueuePathSegment(m_PolygonLinesSourcePoint, _Target, m_PolygonLinesWidth));
    m_PolygonLinesIndexes.push_back((int)m_PolygonLinesIndexes.size());
    m_PolygonLinesSourcePoint = _Target;
}

void RenderingQueuePathBuilder::arc_to(const gs_vec2f& _Target, const float& _Radius)
{
    // calculate ellipse data
    gs_complex<float> perpendicular  = gs_cnormf<float>(gs_complex<float>((_Target - m_PolygonLinesSourcePoint).x, (_Target - m_PolygonLinesSourcePoint).y)) * gs_complex<float>(0.f, 1.f);
    gs_vec2f          center         = (_Target + m_PolygonLinesSourcePoint) * 0.5f + gs_vec2f(gs_realf(perpendicular), gs_imagf(perpendicular)) * _Radius;
    float             sourceAngle    = gs_to_degrees(atan2((m_PolygonLinesSourcePoint - center).y, (m_PolygonLinesSourcePoint - center).x));
    float             targetAngle    = gs_to_degrees(atan2((_Target - center).y, (_Target - center).x));
    float             radius         = (float)gs_vector_length(center - _Target);
    int               segmentsCount  = 36; // TODO: how to compute approximate ellipse segments count ???
    
    if(gs_abs(targetAngle - sourceAngle) < 1e-3) return;
    
    float angleIncrement = gs_abs(360.f / segmentsCount);

    for (float angle = sourceAngle; angle <= targetAngle; angle += angleIncrement)
        line_to(gs_vec2f(center.x + radius * cos(gs_to_radians(angle)), center.y + radius * sin(gs_to_radians(angle))));
    line_to(_Target);
}

void RenderingQueuePathBuilder::build_mesh(
    const gs_color&                                          _Color,
    const float&                                             _Width,
    std::vector<ApplicationRenderingBackendMeshVertex>&          _Vertexes,
    std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes,
    const ApplicationRenderingBackendTexture&                _Texture)
{
    if(m_PolygonLines.empty())
    {
        end();
        return;
    }

    float width = gs_max(_Width, m_PolygonLinesWidth);

    if (m_PolygonLines.size() == 1)
    {
        m_PolygonLines[0].setup(width);

        build_triangle_filled_mesh(
            m_PolygonLines[0].P1min,
            m_PolygonLines[0].P2min,
            m_PolygonLines[0].P1max,
            _Color,
            _Texture,
            _Vertexes,
            _Indexes);

        build_triangle_filled_mesh(
            m_PolygonLines[0].P2min,
            m_PolygonLines[0].P2max,
            m_PolygonLines[0].P1max,
            _Color,
            _Texture,
            _Vertexes,
            _Indexes);
        
        end();
        return;
    }
    
    auto get_element = [](const int& _Index, const int& _Size)->int
    {
        int index = _Index;
        while (index < 0)      index += _Size;
        while (index >= _Size) index -= _Size;
        return index;
    };

    const bool pathIsClosed = gs_vector_length(m_PolygonLines[0].P1 - m_PolygonLines[m_PolygonLines.size() - 1].P2) < gs_epsilon<float>();

    for (int i = 0; i < (int)m_PolygonLines.size(); i++)
    {
        m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].setup(width);

        build_triangle_filled_mesh(
            m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].P1min,
            m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].P2min,
            m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].P1max,
            _Color,
            _Texture,
            _Vertexes,
            _Indexes);

        build_triangle_filled_mesh(
            m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].P2min,
            m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].P2max,
            m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].P1max,
            _Color,
            _Texture,
            _Vertexes,
            _Indexes);

        // TODO: need another algorithm of lines smoothing
        if(i-1 >= 0 || pathIsClosed)
        {
            m_PolygonLines[get_element(i-1, (int)m_PolygonLines.size())].setup(width);

            build_triangle_filled_mesh(
                m_PolygonLines[i].P1max,
                m_PolygonLines[get_element(i-1, (int)m_PolygonLines.size())].P2max,
                m_PolygonLines[get_element(i-1, (int)m_PolygonLines.size())].P2,
                _Color,
                _Texture,
                _Vertexes,
                _Indexes);

            build_triangle_filled_mesh(
                m_PolygonLines[i].P1min,
                m_PolygonLines[get_element(i-1, (int)m_PolygonLines.size())].P2min,
                m_PolygonLines[get_element(i-1, (int)m_PolygonLines.size())].P2,
                _Color,
                _Texture,
                _Vertexes,
                _Indexes);
        }
    }

    end();
}

void RenderingQueuePathBuilder::build_mesh_filled(
    const gs_color&                                          _Color,
    std::vector<ApplicationRenderingBackendMeshVertex>&          _Vertexes,
    std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes,
    const ApplicationRenderingBackendTexture&                _Texture)
{
    // auxiliary lambdas
    auto get_element = [](const int& _Index, const int& _Size)->int
    {
        int index = _Index;
        while (index < 0)      index += _Size;
        while (index >= _Size) index -= _Size;
        return index;
    };

    // checks

    // no triangles
    if((int)m_PolygonLines.size() < 3)
    {
        end();
        return;
    }

    // triangle
    if((int)m_PolygonLines.size() == 3)
    {
        build_triangle_filled_mesh(
            m_PolygonLines[0].P1,
            m_PolygonLines[1].P1,
            m_PolygonLines[2].P1,
            _Color,
            _Texture,
            _Vertexes,
            _Indexes);

        end();
        return;
    }

    // setup lines indexes
    for (int i = 0; i < (int)m_PolygonLines.size(); i++)
        m_PolygonLines[i].Index = i;

    // triangulate polygon
    while (m_PolygonLinesIndexes.size() > 2)
    {
        for (int i = 0; i < (int)m_PolygonLinesIndexes.size(); i++)
        {
            // construct triangle
            int      ia = m_PolygonLinesIndexes[get_element(i, (int)m_PolygonLinesIndexes.size())    ];
            int      ib = m_PolygonLinesIndexes[get_element(i - 1, (int)m_PolygonLinesIndexes.size())];
            int      ic = m_PolygonLinesIndexes[get_element(i + 1, (int)m_PolygonLinesIndexes.size())];
            gs_vec2f pa = m_PolygonLines[ia].P1;
            gs_vec2f pb = m_PolygonLines[ib].P1;
            gs_vec2f pc = m_PolygonLines[ic].P1;

            // detect if this triangle is an ear, i.e there are no other points besides
            // this triangle points that are inside this triangle
            bool isEar = true;

            for (int j = 0; j < (int)m_PolygonLines.size(); j++)
            {
                if( m_PolygonLines[j].Index == m_PolygonLines[ia].Index ||
                    m_PolygonLines[j].Index == m_PolygonLines[ib].Index ||
                    m_PolygonLines[j].Index == m_PolygonLines[ic].Index) continue;

                gs_vec2f poly[3] = {pa, pb, pc};

                if(gs_point_in_2D_polygon(poly, 3, gs_vec2f(m_PolygonLines[j].P1)))
                {
                    isEar = false;
                    break;
                }
            }

            if(isEar || m_PolygonLinesIndexes.size() <= 3)
            {
                build_triangle_filled_mesh(pb, pa, pc, _Color, _Texture, _Vertexes, _Indexes);
                m_PolygonLinesIndexes.erase(m_PolygonLinesIndexes.begin() + i);
                break;
            }
        }
    }

    end();
}

void RenderingQueuePathBuilder::end()
{
    m_PolygonLines.clear();
    m_PolygonLinesIndexes.clear();
}

void RenderingQueuePathBuilder::build_triangle_filled_mesh(
    const gs_vec2f&                                          _P1,
    const gs_vec2f&                                          _P2,
    const gs_vec2f&                                          _P3,
    const gs_color&                                          _Color,
    const ApplicationRenderingBackendTexture&                _Texture,
    std::vector<ApplicationRenderingBackendMeshVertex>&          _Vertexes,
    std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes)
{
    const int size = (int)_Vertexes.size();

    _Vertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            gs_vec3f(_P1.x, _P1.y, 0.f),
            gs_vec3f(0.f), gs_vec2f(_P1.x / _Texture.Width, _P1.y / _Texture.Height),
            _Color));
    
    _Vertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            gs_vec3f(_P2.x, _P2.y, 0.f),
            gs_vec3f(0.f), gs_vec2f(_P2.x / _Texture.Width, _P2.y / _Texture.Height),
            _Color));
    
    _Vertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            gs_vec3f(_P3.x, _P3.y, 0.f),
            gs_vec3f(0.f),
            gs_vec2f(_P3.x / _Texture.Width, _P3.y / _Texture.Height),
            _Color));
    
    for (int i = size; i < (int)_Vertexes.size(); ++i)
        _Indexes.push_back(i);
}

// RenderingQueue
RenderingQueue::RenderingQueue(){}
RenderingQueue::~RenderingQueue(){}

RenderingQueueMetrics RenderingQueue::get_rendering_queue_metrics() const
{
    return m_Metrics;
}

float RenderingQueue::get_minimum_line_width() const
{
    return m_MinimumLineWidth;
}

gs_vec3f RenderingQueue::get_cursor_postion() const
{
    gs_vec2f size   = ApplicationPlatformBackend::get_window_size();
    gs_vec2f cursor = ApplicationPlatformBackend::get_window_cursor_position();
    gs_mat4f matrix =
        gs_matrix_invert_square(m_CameraViewMatrix) *
        gs_matrix_invert_square(m_ProjectionMatrix);
    
    return matrix * gs_vec4f(gs_vector_convert_to_NDC(cursor, size), -1.f, 1.f);
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
    GS_ASSERT(m_ClearColors.empty());
    GS_ASSERT(m_ClippingBoxes.empty());
    GS_ASSERT(m_MeshVertexes.empty());
    GS_ASSERT(m_MeshVertexesIndexes.empty());
    GS_ASSERT(m_VertexesOffset == 0);
    GS_ASSERT(m_IndexesOffset == 0);

    // metrics
    m_FrameRateMeasurementStartTimePoint = Frenchie::Core::Clock::tic();

    // push clear color
    push_clear_color(gs_color_rgba(150, 150, 150, 150));
    push_clip_box(gs_2dboxf(gs_vec2f(0.f, 0.f), ApplicationPlatformBackend::get_window_size()));

    // compute projection matrix
    float width  = ApplicationPlatformBackend::get_window_size().x;
    float height = ApplicationPlatformBackend::get_window_size().y;

    auto camera = ApplicationRenderingBackend::calculate_2d_camera_view_and_projection(
        gs_vec2f(width * 0.5f, height * 0.5f),
        gs_vec3f(0.f, 1.f, 0.f),
        gs_vec3f(0.f, 0.f, -1.f),
        gs_vec2f(width, height),
        0.f,
        get_near_plane(),
        get_far_plane()
    );

    m_CameraViewMatrix = camera.CameraView;
    m_ProjectionMatrix = camera.Projection;

    // compute viewport
    gs_vec3f viewportMin = gs_vector_convert_to_NDC(gs_vec2f(0.f, 0.f), gs_vec2f(width, height));
    gs_vec3f viewportMax = gs_vector_convert_to_NDC(ApplicationPlatformBackend::get_window_size(), gs_vec2f(width, height));

    m_Viewport = gs_2dboxf(
        gs_matrix_invert_square(m_ProjectionMatrix) * gs_matrix_invert_square(m_CameraViewMatrix) * gs_vec4f(viewportMin, 1.f),
        gs_matrix_invert_square(m_ProjectionMatrix) * gs_matrix_invert_square(m_CameraViewMatrix) * gs_vec4f(viewportMax, 1.f));
}

void RenderingQueue::frame_update()
{
}

void RenderingQueue::frame_render()
{
    if(!ApplicationRenderingBackend::begin_render())
        return;

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

    // compute projection matrix
    gs_mat4f projectionMatrix = m_ProjectionMatrix * m_CameraViewMatrix;

    // execute rendering commands
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
            ApplicationRenderingBackend::scissor_box(gs_2dboxf(clippingRect.Min * displayScale, clippingRect.Max * displayScale));
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
            auto texture            = renderingCommand.value().Texture;
            auto transformMatrix    = renderingCommand.value().Transform;
            auto meshRenderingHints = renderingCommand.value().MeshRendererHints;

            ApplicationRenderingBackend::render_mesh(
                &m_MeshVertexes[0],
                (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size(),
                mesh.VertexesCount,
                mesh.VertexesOffset,
                &m_MeshVertexesIndexes[0],
                (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexesIndexes.size(),
                mesh.IndexesCount,
                mesh.IndexesOffset,
                texture,
                projectionMatrix * transformMatrix,
                meshRenderingHints);
        }
    }

    // save metrics
    m_Metrics.RenderingCommandsCount = (int)m_Commands.size();
    m_Metrics.RenderedTrianglesCount = (int)(m_MeshVertexes.size() / 3);
    double current = (double)1e9 / Frenchie::Core::Clock::elapsed<std::chrono::nanoseconds>(m_FrameRateMeasurementStartTimePoint, Frenchie::Core::Clock::tic());
    m_FrameRateMeasurementFilterBuffer.push(current);
    m_Metrics.FrameRate += (current - m_FrameRateMeasurementFilterBuffer.at(m_FrameRateMeasurementFilterBuffer.size() - 1)) / (double)(m_FrameRateMeasurementFilterBuffer.size());

    // clear commands queue
    m_Commands.clear();

    // end rendering
    ApplicationRenderingBackend::end_render();
}

void RenderingQueue::frame_finish()
{
    // clear rendering data
    m_ClearColors.clear();
    m_ClippingBoxes.clear();
    m_MeshVertexes.clear();
    m_MeshVertexesIndexes.clear();

    // restore mesh offsets
    m_IndexesOffset  = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();
    m_VertexesOffset = (int)m_MeshVertexesIndexes.size();
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
    const gs_mat4f&                                             _Transform,
    const ApplicationRenderingBackendGraphicsApiRenderingHints& _MeshRenderingHints)
{
    push_rendering_command(
        ApplicationRenderingBackend::get_default_texture(),
        gs_color_rgba(255, 255, 255, 255),
        _Transform,
        _MeshRenderingHints);
}

void RenderingQueue::push_rendering_command(
    const ApplicationRenderingBackendTexture&                   _Texture,
    const gs_color&                                             _Color,
    const gs_mat4f&                                             _Transform,
    const ApplicationRenderingBackendGraphicsApiRenderingHints& _MeshRenderingHints)
{
    if(m_MeshVertexesIndexes.empty() || m_MeshVertexes.empty()) return;

    push_rendering_command(

        // construct mesh
        RenderingQueueMesh(
            (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size(),
            m_VertexesOffset,
            (int)m_MeshVertexesIndexes.size(),
            m_IndexesOffset),

        // setup texture
        ApplicationRenderingBackendTexture(
            _Texture.Ptr,
            _Texture.Width,
            _Texture.Height,
            _Color,
            _Texture.Format,
            _Texture.Wrap,
            _Texture.MinFilter,
            _Texture.MaxFilter),
        _Transform,
        _MeshRenderingHints,
        current_clear_color(),
        current_clipping_box());

    // move offsets
    m_IndexesOffset  = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();
    m_VertexesOffset = (int)m_MeshVertexesIndexes.size();
}

void RenderingQueue::push_rendering_command(
    const RenderingQueueMesh&                                   _Mesh,
    const ApplicationRenderingBackendTexture&                   _Texture,
    const gs_mat4f&                                             _Transform,
    const ApplicationRenderingBackendGraphicsApiRenderingHints& _RendererHints,
    const gs_color&                                             _ClearColor,
    const gs_2dboxf&                                            _ClippinBox)
{
    m_Commands.push_back(
        RenderingQueueCommand(
            RenderingQueueRenderingCommand(
                _Mesh,
                _Texture,
                _Transform,
                _RendererHints),
        RenderingQueueRendererCommandClearColor(_ClearColor),
        RenderingQueueRendererCommandClippingBox(_ClippinBox)));
}

void RenderingQueue::push_clip_box(const gs_2dboxf& _Value, const gs_mat4f& _Transform)
{
    gs_2dboxf clipRect = _Value.transform(_Transform);
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

gs_2dboxf RenderingQueue::current_clipping_box() const
{
    return !m_ClippingBoxes.empty() ? m_ClippingBoxes[m_ClippingBoxes.size() - 1] : gs_2dboxf(gs_vec2f(0.f, 0.f), ApplicationPlatformBackend::get_window_size());
}

gs_2dboxf RenderingQueue::current_viewport() const
{
    return m_Viewport;
}

gs_color RenderingQueue::current_clear_color() const
{
    return !m_ClearColors.empty() ?
        m_ClearColors[m_ClearColors.size() - 1] :
            gs_color_rgba(255, 255, 255, 255);
}

gs_mat4f RenderingQueue::calculate_transform_matrix(const float& _Depth, const gs_vec2f& _Position, const float& _Rotation, const gs_vec2f& _Scale)
{
    gs_mat4f matrix(1.f);

    return gs_matrix_translate(matrix, gs_vec3f(_Position, _Depth)) *
            gs_matrix_rotate(matrix, gs_to_radians(_Rotation), gs_vec3f(0.f, 0.f, 1.f)) * 
            gs_matrix_scale(matrix, gs_vec3f(_Scale, 1.f));
}

gs_vec2f RenderingQueue::calculate_arc_point(const gs_vec2f& _Center, const float& _MinorRadius, const float& _MajorRadius, const float&    _ArcAngle)
{
    return gs_vec2f(
        _Center.x + _MinorRadius * cos(gs_to_radians(_ArcAngle)),
        _Center.y + _MajorRadius * sin(gs_to_radians(_ArcAngle)));
}

void RenderingQueue::push_triangle_filled(
    const gs_vec2f&                           _P1,
    const gs_vec2f&                           _P2,
    const gs_vec2f&                           _P3,
    const gs_color&                           _Color,
    const gs_mat4f&                           _Transform,
    const ApplicationRenderingBackendTexture& _Texture)
{
    if( !current_clipping_box().contains(_Transform * gs_vec4f(_P1, 0.f, 1.f)) &&
        !current_clipping_box().contains(_Transform * gs_vec4f(_P2, 0.f, 1.f)) &&
        !current_clipping_box().contains(_Transform * gs_vec4f(_P3, 0.f, 1.f)))
    {
        return;
    }

    build_triangle_filled_mesh(_P1, _P2, _P3, _Color, !_Texture.is_null() ? _Texture : ApplicationRenderingBackend::get_default_texture());
    push_rendering_command(!_Texture.is_null() ? _Texture : ApplicationRenderingBackend::get_default_texture(), _Color, _Transform);
}

void RenderingQueue::push_rectangle_filled(
    const gs_vec2f&                           _Min,
    const gs_vec2f&                           _Max,
    const gs_color&                           _Color,
    const gs_mat4f&                           _Transform,
    const ApplicationRenderingBackendTexture& _Texture)
{
    if(!current_clipping_box().overlaps(gs_2dboxf(_Transform * gs_vec4f(_Min, 0.f, 1.f), _Transform * gs_vec4f(_Max, 0.f, 1.f))))
        return;

    build_rectangle_filled_mesh(_Min, _Max, _Color, !_Texture.is_null() ? _Texture : ApplicationRenderingBackend::get_default_texture());
    push_rendering_command(!_Texture.is_null() ? _Texture : ApplicationRenderingBackend::get_default_texture(), _Color, _Transform);
}

void RenderingQueue::push_rectangle_gradient_mesh(
    const gs_vec2f& _Min,
    const gs_vec2f& _Max,
    const gs_color& _Color1,
    const gs_color& _Color2,
    const gs_color& _Color3,
    const gs_color& _Color4,
    const gs_mat4f& _Transform)
{
    if(!current_clipping_box().overlaps(gs_2dboxf(_Transform * gs_vec4f(_Min, 0.f, 1.f), _Transform * gs_vec4f(_Max, 0.f, 1.f))))
        return;

    build_rectangle_gradient_mesh(_Min, _Max, _Color1, _Color2, _Color3, _Color4);
    push_rendering_command(_Transform);
}

void RenderingQueue::push_rectangle_rounded_filled(
    const gs_vec2f& _Min,
    const gs_vec2f& _Max,
    const float&    _Radius,
    const gs_color& _Color,
    const gs_mat4f& _Transform,
    bool            _RoundTopLeftCorner,
    bool            _RoundTopRightCorner,
    bool            _RoundBottomRightCorner,
    bool            _RoundBottomLeftCorner)
{
    if(!current_clipping_box().overlaps(gs_2dboxf(_Transform * gs_vec4f(_Min, 0.f, 1.f), _Transform * gs_vec4f(_Max, 0.f, 1.f))))
        return;

    // check rounding radius
    if(_Radius <= m_MinimumLineWidth ||
        (!_RoundTopLeftCorner     &&
         !_RoundTopRightCorner    &&
         !_RoundBottomRightCorner &&
         !_RoundBottomLeftCorner))
    {
        push_rectangle_filled(_Min, _Max, _Color, _Transform);
        return;
    }

    // build mesh
    float radius = gs_min(gs_min(_Radius, gs_abs(_Max.x - _Min.x) * 0.5f), gs_min(_Radius, gs_abs(_Max.y - _Min.y) * 0.5f));

    const gs_vec2f p1 = gs_vec2f(_Min.x, _Min.y);
    const gs_vec2f p2 = gs_vec2f(_Max.x, _Min.y);
    const gs_vec2f p3 = gs_vec2f(_Max.x, _Max.y);
    const gs_vec2f p4 = gs_vec2f(_Min.x, _Max.y);

    gs_vec2f p13 = gs_vector_normalize(p1 - p3);
    gs_vec2f p24 = gs_vector_normalize(p2 - p4);
    p13 = gs_vec2f(gs_sign(p13.x), gs_sign(p13.y));
    p24 = gs_vec2f(gs_sign(p24.x), gs_sign(p24.y));

    if(_RoundTopLeftCorner)
        build_arc_filled_mesh(p1 - p13 * radius, radius, radius, 180.f, 270.f, _Color, ApplicationRenderingBackend::get_default_texture(), 16);
    else
        build_rectangle_filled_mesh(p1, p1 + radius, _Color, ApplicationRenderingBackend::get_default_texture());

    if(_RoundTopRightCorner)
        build_arc_filled_mesh(p2 - p24 * radius, radius, radius, 270.f, 360.f, _Color, ApplicationRenderingBackend::get_default_texture(), 16);
    else
        build_rectangle_filled_mesh(p2 - gs_vec2f(radius, 0.f), p2 + gs_vec2f(0.f, radius), _Color, ApplicationRenderingBackend::get_default_texture());

    if(_RoundBottomRightCorner)
        build_arc_filled_mesh(p3 + p13 * radius, radius, radius, 0.f, 90.f, _Color, ApplicationRenderingBackend::get_default_texture(), 16);
    else
        build_rectangle_filled_mesh(p3 - radius, p3, _Color, ApplicationRenderingBackend::get_default_texture());

    if(_RoundBottomLeftCorner)
        build_arc_filled_mesh(p4 + p24 * radius, radius, radius, 90.f, 180.f, _Color, ApplicationRenderingBackend::get_default_texture(), 16);
    else
        build_rectangle_filled_mesh(p4 - gs_vec2f(0.f, radius), p4 + gs_vec2f(radius, 0.f), _Color, ApplicationRenderingBackend::get_default_texture());
    
    build_rectangle_filled_mesh(p1 - p13 * gs_vec2f(radius, 0.f), p3 + p13 * gs_vec2f(radius, 0.f), _Color, ApplicationRenderingBackend::get_default_texture());
    build_rectangle_filled_mesh(p1 - p13 * gs_vec2f(0.f, radius), p3 + p13 * gs_vec2f(0.f, radius), _Color, ApplicationRenderingBackend::get_default_texture());

    // push rendering command
    push_rendering_command(ApplicationRenderingBackend::get_default_texture(), _Color, _Transform);
}

void RenderingQueue::push_arc_filled(
    const gs_vec2f&                           _Center,
    const float&                              _MinorRadius,
    const float&                              _MajorRadius,
    const float&                              _SourceAngle,
    const float&                              _TargetAngle,
    const gs_color&                           _Color,
    const gs_mat4f&                           _Transform,
    const ApplicationRenderingBackendTexture& _Texture)
{
    // check that we are within viewport
    if(!current_clipping_box().overlaps(
            gs_2dboxf(
                _Transform * gs_vec4f((_Center - gs_vec2f(_MinorRadius, _MajorRadius)), 0.f, 1.f),
                _Transform * gs_vec4f((_Center + gs_vec2f(_MinorRadius, _MajorRadius)), 0.f, 1.f))))
    {
        return;
    }

    build_arc_filled_mesh(
        _Center,
        _MinorRadius,
        _MajorRadius,
        _SourceAngle,
        _TargetAngle,
        _Color,
        !_Texture.is_null() ? _Texture : ApplicationRenderingBackend::get_default_texture());

    push_rendering_command(
        !_Texture.is_null() ? _Texture : ApplicationRenderingBackend::get_default_texture(),
        _Color,
        _Transform);
}

void RenderingQueue::push_line(
    const gs_vec2f& _P1,
    const gs_vec2f& _P2,
    const float&    _Width,
    const gs_color& _Color,
    const gs_mat4f& _Transform)
{
    if(!current_clipping_box().contains(_Transform * gs_vec4f(_P1, 0.f, 1.f)) &&
        !current_clipping_box().contains(_Transform * gs_vec4f(_P2, 0.f, 1.f)))
    {
        return;
    }

    RenderingQueue::build_line_mesh(
        _P1,
        _P2,
        _Width,
        _Color,
        ApplicationRenderingBackend::get_default_texture());

    push_rendering_command(ApplicationRenderingBackend::get_default_texture(), _Color, _Transform);
}

void RenderingQueue::push_arc(
    const gs_vec2f& _Center,
    const float&    _MinorRadius,
    const float&    _MajorRadius,
    const float&    _SourceAngle,
    const float&    _TargetAngle,
    const float&    _Width,
    const gs_color& _Color,
    const gs_mat4f& _Transform)
{
    // check that we are within viewport
    if(!current_clipping_box().overlaps(
            gs_2dboxf(
                _Transform * gs_vec4f((_Center - gs_vec2f(_MinorRadius, _MajorRadius)), 0.f, 1.f),
                _Transform * gs_vec4f((_Center + gs_vec2f(_MinorRadius, _MajorRadius)), 0.f, 1.f))))
    {
        return;
    }

    build_arc_mesh(_Center, _MinorRadius, _MajorRadius, _SourceAngle, _TargetAngle, _Width, _Color);
    push_rendering_command(ApplicationRenderingBackend::get_default_texture(), _Color, _Transform);
}

void RenderingQueue::push_triangle(
    const gs_vec2f& _P1,
    const gs_vec2f& _P2,
    const gs_vec2f& _P3,
    const float&    _Width,
    const gs_color& _Color,
    const gs_mat4f& _Transform)
{
    // check if we are within viewport
    if( !current_clipping_box().contains(_Transform * gs_vec4f(_P1, 0.f, 1.f)) &&
        !current_clipping_box().contains(_Transform * gs_vec4f(_P2, 0.f, 1.f)) &&
        !current_clipping_box().contains(_Transform * gs_vec4f(_P3, 0.f, 1.f)))
    {
        return;
    }

    // TODO: check if path building algorithms are optimized enough to be user every frame...
    // build path
    m_PathBuilder.begin(_P1);
    m_PathBuilder.line_to(_P2);
    m_PathBuilder.line_to(_P3);
    m_PathBuilder.line_to(_P1);

    // build mesh
    m_PathBuilder.build_mesh(
        _Color,
        _Width,
        m_MeshVertexes,
        m_MeshVertexesIndexes,
        ApplicationRenderingBackend::get_default_texture());

    // push rendering command
    push_rendering_command(_Transform, _Color);
}

void RenderingQueue::push_rectangle(
    const gs_vec2f& _Min,
    const gs_vec2f& _Max,
    const float&    _Width,
    const gs_color& _Color,
    const gs_mat4f& _Transform)
{
    if(!current_clipping_box().overlaps(
        gs_2dboxf(
            _Transform * gs_vec4f(_Min, 0.f, 1.f, 1.f),
            _Transform * gs_vec4f(_Max, 0.f, 1.f, 1.f))))
    {
        return;
    }

    // TODO: check if path building algorithms are optimized enough to be user every frame...
    // build path
    m_PathBuilder.begin(_Min);
    m_PathBuilder.line_to(gs_vec2f(_Max.x, _Min.y));
    m_PathBuilder.line_to(_Max);
    m_PathBuilder.line_to(gs_vec2f(_Min.x, _Max.y));
    m_PathBuilder.line_to(_Min);

    // build mesh
    m_PathBuilder.build_mesh(
        _Color,
        gs_max(_Width, m_MinimumLineWidth),
        m_MeshVertexes,
        m_MeshVertexesIndexes,
        ApplicationRenderingBackend::get_default_texture());

    // push rendering command
    push_rendering_command(_Transform);
}

void RenderingQueue::push_rectangle_rounded(
    const gs_vec2f& _Min,
    const gs_vec2f& _Max,
    const float&    _Radius,
    const float&    _Width,
    const gs_color& _Color,
    const gs_mat4f& _Transform)
{
    if(!current_clipping_box().overlaps(
        gs_2dboxf(
            _Transform * gs_vec4f(_Min, 0.f, 1.f, 1.f),
            _Transform * gs_vec4f(_Max, 0.f, 1.f, 1.f))))
    {
        return;
    }

    // check rounding radius
    if(_Radius <= gs_max(_Width, 4.f))
    {
        push_rectangle(_Min, _Max, _Width, _Color, _Transform);
        return;
    }

    // compute radius
    float radius = gs_min(gs_min(_Radius, gs_abs(_Max.x - _Min.x) * 0.5f), gs_min(_Radius, gs_abs(_Max.y - _Min.y) * 0.5f));

    // points
    gs_vec2f TL = gs_vec2f(_Min.x + radius, _Max.y - radius);
    gs_vec2f BL = gs_vec2f(_Min.x + radius, _Min.y + radius);
    gs_vec2f TR = gs_vec2f(_Max.x - radius, _Max.y - radius);
    gs_vec2f BR = gs_vec2f(_Max.x - radius, _Min.y + radius);

    // sides
    build_arc_mesh(TL, radius, radius, 90.f, 180.f, _Width, _Color);
    build_arc_mesh(BL, radius, radius, 180.f, 270.f, _Width, _Color);
    build_arc_mesh(TR, radius, radius, 0.f, 90.f, _Width, _Color);
    build_arc_mesh(BR, radius, radius, 270.f, 360.f, _Width, _Color);
    build_line_mesh(calculate_arc_point(TL, radius, radius, 180), calculate_arc_point(BL, radius, radius, 180), _Width, _Color, ApplicationRenderingBackend::get_default_texture());
    build_line_mesh(calculate_arc_point(TL, radius, radius, 90), calculate_arc_point(TR, radius, radius, 90), _Width, _Color, ApplicationRenderingBackend::get_default_texture());
    build_line_mesh(calculate_arc_point(TR, radius, radius, 0), calculate_arc_point(BR, radius, radius, 0), _Width, _Color, ApplicationRenderingBackend::get_default_texture());
    build_line_mesh(calculate_arc_point(BL, radius, radius, 270), calculate_arc_point(BR, radius, radius, 270), _Width, _Color, ApplicationRenderingBackend::get_default_texture());

    push_rendering_command(ApplicationRenderingBackend::get_default_texture(), _Color, _Transform);
}

void RenderingQueue::build_triangle_filled_mesh(
    const gs_vec2f&                           _P1,
    const gs_vec2f&                           _P2,
    const gs_vec2f&                           _P3,
    const gs_color&                           _Color,
    const ApplicationRenderingBackendTexture& _Texture)
{
    const ApplicationRenderingBackendMeshVertexIndex size = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();

    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            gs_vec3f(_P1.x, _P1.y, 0.f),
            gs_vec3f(0.f), gs_vec2f(_P1.x / _Texture.Width, _P1.y / _Texture.Height),
            _Color));
    
    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            gs_vec3f(_P2.x, _P2.y, 0.f),
            gs_vec3f(0.f), gs_vec2f(_P2.x / _Texture.Width, _P2.y / _Texture.Height),
            _Color));
    
    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            gs_vec3f(_P3.x, _P3.y, 0.f),
            gs_vec3f(0.f),
            gs_vec2f(_P3.x / _Texture.Width, _P3.y / _Texture.Height),
            _Color));
    
    for (ApplicationRenderingBackendMeshVertexIndex i = size; i < (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size(); ++i)
        m_MeshVertexesIndexes.push_back(i);
}

void RenderingQueue::build_triangle_gradient_mesh(
    const gs_vec2f& _P1,
    const gs_vec2f& _P2,
    const gs_vec2f& _P3,
    const gs_color& _Color1,
    const gs_color& _Color2,
    const gs_color& _Color3)
{
    const ApplicationRenderingBackendMeshVertexIndex size = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();

    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            gs_vec3f(_P1.x, _P1.y, 0.f),
            gs_vec3f(0.f),
            gs_vec3f(0.f),
            _Color1));
    
    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            gs_vec3f(_P2.x, _P2.y, 0.f),
            gs_vec3f(0.f),
            gs_vec3f(0.f),
            _Color2));
    
    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            gs_vec3f(_P3.x, _P3.y, 0.f),
            gs_vec3f(0.f),
            gs_vec3f(0.f),
            _Color3));
    
    for (ApplicationRenderingBackendMeshVertexIndex i = size; i < (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size(); ++i)
        m_MeshVertexesIndexes.push_back(i);
}

void RenderingQueue::build_rectangle_filled_mesh(
    const gs_vec2f&                           _Min,
    const gs_vec2f&                           _Max,
    const gs_color&                           _Color,
    const ApplicationRenderingBackendTexture& _Texture)
{
    const ApplicationRenderingBackendMeshVertexIndex size   = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();
    const gs_vec3f _P1 = gs_vec3f(_Min.x, _Min.y, 0.f);
    const gs_vec3f _P2 = gs_vec3f(_Max.x, _Min.y, 0.f);
    const gs_vec3f _P3 = gs_vec3f(_Max.x, _Max.y, 0.f);
    const gs_vec3f _P4 = gs_vec3f(_Min.x, _Max.y, 0.f);

    const gs_vec2f _MinUV = gs_vec2f(0.f, 0.f);
    const gs_vec2f _MaxUV = gs_vec2f(1.f, 1.f);

    const gs_vec3f _UV1 = gs_vec3f(_MinUV.x, _MinUV.y, 0.f);
    const gs_vec3f _UV2 = gs_vec3f(_MaxUV.x, _MinUV.y, 0.f);
    const gs_vec3f _UV3 = gs_vec3f(_MaxUV.x, _MaxUV.y, 0.f);
    const gs_vec3f _UV4 = gs_vec3f(_MinUV.x, _MaxUV.y, 0.f);

    // triangle 1
    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P1,
            gs_vec3f(0.f),
            gs_vec2f(_UV1.x, _UV1.y),
            _Color));

    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P2,
            gs_vec3f(0.f),
            gs_vec2f(_UV2.x, _UV2.y),
            _Color));

    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P4,
            gs_vec3f(0.f),
            gs_vec2f(_UV4.x, _UV4.y),
            _Color));

    // triangle 2
    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P2,
            gs_vec3f(0.f),
            gs_vec2f(_UV2.x, _UV2.y),
            _Color));

    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P3,
            gs_vec3f(0.f),
            gs_vec2f(_UV3.x, _UV3.y),
            _Color));

    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P4,
            gs_vec3f(0.f),
            gs_vec2f(_UV4.x, _UV4.y),
            _Color));

    for (ApplicationRenderingBackendMeshVertexIndex i = size; i < (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size(); ++i)
        m_MeshVertexesIndexes.push_back(i);
}

void RenderingQueue::build_rectangle_filled_mesh(
    const gs_vec2f& _Min,
    const gs_vec2f& _Max,
    const gs_vec2f& _MinUV,
    const gs_vec2f& _MaxUV,
    const gs_color& _Color)
{
    const ApplicationRenderingBackendMeshVertexIndex size = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();

    const gs_vec3f _P1 = gs_vec3f(_Min.x, _Min.y, 0.f);
    const gs_vec3f _P2 = gs_vec3f(_Max.x, _Min.y, 0.f);
    const gs_vec3f _P3 = gs_vec3f(_Max.x, _Max.y, 0.f);
    const gs_vec3f _P4 = gs_vec3f(_Min.x, _Max.y, 0.f);

    const gs_vec3f _UV1 = gs_vec3f(_MinUV.x, _MinUV.y, 0.f);
    const gs_vec3f _UV2 = gs_vec3f(_MaxUV.x, _MinUV.y, 0.f);
    const gs_vec3f _UV3 = gs_vec3f(_MaxUV.x, _MaxUV.y, 0.f);
    const gs_vec3f _UV4 = gs_vec3f(_MinUV.x, _MaxUV.y, 0.f);

    // triangle 1
    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P1,
            gs_vec3f(0.f),
            gs_vec2f(_UV1.x, _UV1.y),
            _Color));

    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P2,
            gs_vec3f(0.f),
            gs_vec2f(_UV2.x, _UV2.y),
            _Color));

    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P4,
            gs_vec3f(0.f),
            gs_vec2f(_UV4.x, _UV4.y),
            _Color));

    // triangle 2
    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P2,
            gs_vec3f(0.f),
            gs_vec2f(_UV2.x, _UV2.y),
            _Color));

    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P3,
            gs_vec3f(0.f),
            gs_vec2f(_UV3.x, _UV3.y),
            _Color));

    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P4,
            gs_vec3f(0.f),
            gs_vec2f(_UV4.x, _UV4.y),
            _Color));

    for (ApplicationRenderingBackendMeshVertexIndex i = size; i < (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size(); ++i)
        m_MeshVertexesIndexes.push_back(i);
}

void RenderingQueue::build_rectangle_gradient_mesh(
    const gs_vec2f& _Min,
    const gs_vec2f& _Max,
    const gs_color& _Color1,
    const gs_color& _Color2,
    const gs_color& _Color3,
    const gs_color& _Color4)
{
    const ApplicationRenderingBackendMeshVertexIndex size = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();

    const gs_vec3f _P1 = gs_vec3f(_Min.x, _Min.y, 0.f);
    const gs_vec3f _P2 = gs_vec3f(_Max.x, _Min.y, 0.f);
    const gs_vec3f _P3 = gs_vec3f(_Max.x, _Max.y, 0.f);
    const gs_vec3f _P4 = gs_vec3f(_Min.x, _Max.y, 0.f);

    // triangle 1
    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P1,
            gs_vec3f(0.f),
            gs_vec2f(0.f),
            _Color1));

    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P2,
            gs_vec3f(0.f),
            gs_vec2f(0.f),
            _Color2));

    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P4,
            gs_vec3f(0.f),
            gs_vec2f(0.f),
            _Color4));

    // triangle 2
    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P2,
            gs_vec3f(0.f),
            gs_vec2f(0.f),
            _Color2));

    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P3,
            gs_vec3f(0.f),
            gs_vec2f(0.f),
            _Color3));

    m_MeshVertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            _P4,
            gs_vec3f(0.f),
            gs_vec2f(0.f),
            _Color4));

    for (ApplicationRenderingBackendMeshVertexIndex i = size; i < (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size(); ++i)
        m_MeshVertexesIndexes.push_back(i);
}

void RenderingQueue::build_arc_filled_mesh(
    const gs_vec2f&                           _Center,
    const float&                              _MinorRadius,
    const float&                              _MajorRadius,
    const float&                              _SourceAngle,
    const float&                              _TargetAngle,
    const gs_color&                           _Color,
    const ApplicationRenderingBackendTexture& _Texture,
    const int&                                _SegmentsCount)
{
    const float delta = 360.f / _SegmentsCount;

    for (float angle = _SourceAngle; angle < _TargetAngle; angle += delta)
    {        
        build_triangle_filled_mesh(
            _Center,
            gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(angle)), _Center.y + _MajorRadius * sin(gs_to_radians(angle))),
            gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(angle + delta)), _Center.y + _MajorRadius * sin(gs_to_radians(angle + delta))),
            _Color, _Texture);
    }
}

void RenderingQueue::build_line_mesh(
    const gs_vec2f&                           _P1,
    const gs_vec2f&                           _P2,
    const float&                              _Width,
    const gs_color&                           _Color,
    const ApplicationRenderingBackendTexture& _Texture)
{
    const gs_vec3f p1    = gs_vec3f(_P1.x, _P1.y, 0.f);
    const gs_vec3f p2    = gs_vec3f(_P2.x, _P2.y, 0.f);
    float          width = gs_max(_Width, m_MinimumLineWidth);

    gs_vec3f direction     = gs_vector_normalize(_P2 - _P1);
    gs_vec2f perpendicular = gs_vector_normalize(gs_vector_cross(direction, gs_vec3f(0.f, 0.f, 1.f))) * width * 0.5f;

    build_arc_filled_mesh(_P1, width * 0.5f, width * 0.5f, 0.f, 360.f, _Color, _Texture, 8);
    build_arc_filled_mesh(_P2, width * 0.5f, width * 0.5f, 0.f, 360.f, _Color, _Texture, 8);

    build_triangle_filled_mesh(
        _P1 - perpendicular,
        _P2 - perpendicular,
        _P1 + perpendicular,
        _Color,
        _Texture);

    build_triangle_filled_mesh(
        _P2 - perpendicular,
        _P2 + perpendicular,
        _P1 + perpendicular,
        _Color,
        _Texture);
}

void RenderingQueue::build_arc_mesh(
    const gs_vec2f& _Center,
    const float&    _MinorRadius,
    const float&    _MajorRadius,
    const float&    _SourceAngle,
    const float&    _TargetAngle,
    const float&    _Width,
    const gs_color& _Color,
    const int&      _SegmentsCount)
{
    const float delta = 360.f / _SegmentsCount;
    const float width = gs_max(_Width, m_MinimumLineWidth);

    for (float angle = _SourceAngle; angle < _TargetAngle; angle += delta)
    {
        build_line_mesh(
            gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(angle)), _Center.y + _MajorRadius * sin(gs_to_radians(angle))),
            gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(angle + delta)), _Center.y + _MajorRadius * sin(gs_to_radians(angle + delta))),
            width,
            _Color,
            ApplicationRenderingBackend::get_default_texture());
    }

    // TODO: check if path building algorithms are optimized enough to be user every frame...
    // // build path
    // m_PathBuilder.begin(gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(_SourceAngle)), _Center.y + _MajorRadius * sin(gs_to_radians(_SourceAngle))));

    // for (float angle = _SourceAngle + delta; angle < _TargetAngle; angle += delta)
    //     m_PathBuilder.line_to(gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(angle)), _Center.y + _MajorRadius * sin(gs_to_radians(angle))));

    // // build mesh
    // m_PathBuilder.build_mesh(
    //     _Color,
    //     ApplicationRenderingBackend::get_default_texture(),
    //     _Width,
    //     m_MeshVertexes,
    //     m_MeshVertexesIndexes);
}
