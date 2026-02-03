#include <FrenchieApplicationImmediate2DRenderer.hpp>

// Core
#include <FrenchieCoreStringUtilities.hpp>

using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        template<typename Type>
        auto calculate_2d_camera_view_and_projection(
            const gs_vector<Type, 2>& _CameraWorldPosition,
            const gs_vector<Type, 3>& _CameraWorldUpAxisDirection,
            const gs_vector<Type, 3>& _CameraWorldFrontAxisDirection,
            const gs_vector<Type, 2>& _CameraResolution,
            const float&              _CameraRotationAngle,
            const Type&               _CameraNearPlanePosition,
            const Type&               _CameraFarPlanePosition)
        {
            // compute projection matrix
            // float left   = -_CameraResolution.x * 0.5f + _CameraWorldPosition.x;
            // float right  = +_CameraResolution.x * 0.5f + _CameraWorldPosition.x;
            // float bottom = -_CameraResolution.y * 0.5f + _CameraWorldPosition.y;
            // float top    = +_CameraResolution.y * 0.5f + _CameraWorldPosition.y;

            float left   = -_CameraResolution.x * 0.5f + _CameraWorldPosition.x;
            float right  = +_CameraResolution.x * 0.5f + _CameraWorldPosition.x;
            float bottom = +_CameraResolution.y * 0.5f + _CameraWorldPosition.y;
            float top    = -_CameraResolution.y * 0.5f + _CameraWorldPosition.y;

            // camera orientation
            gs_vector<Type, 3> cameraLocalFrontAxisDirection = gs_vector_normalize(_CameraWorldFrontAxisDirection);
            gs_vector<Type, 3> cameraLocalRightAxisDirection = gs_vector_normalize(gs_vector_cross(cameraLocalFrontAxisDirection, _CameraWorldUpAxisDirection));
            gs_vector<Type, 3> cameraLocalUpAxisDirection    = gs_vector_normalize(gs_vector_cross(cameraLocalRightAxisDirection, cameraLocalFrontAxisDirection));

            gs_matrix<Type, 4, 4> cameraview =
                gs_matrix_look_at(
                    gs_vector<Type, 3>(0.f, 0.f, 1),
                    gs_vector<Type, 3>(0.f, 0.f, 1) + cameraLocalFrontAxisDirection, cameraLocalUpAxisDirection);
            
            gs_matrix<Type, 4, 4> projection =
                gs_matrix_ortho(
                    left,
                    right,
                    bottom,
                    top,
                    _CameraNearPlanePosition,
                    _CameraFarPlanePosition) * gs_matrix_rotate(gs_matrix<Type, 4, 4>(1.f), gs_to_radians(_CameraRotationAngle), gs_vector<Type, 3>(0.f, 0.f, 1.f));

            struct
            {
                gs_matrix<Type, 4, 4> cameraview;
                gs_matrix<Type, 4, 4> projection;
            } result = {cameraview, projection};

            return result;
        }
    }
}

int calculate_arc_segments_number(float _Radius, float _MaximumError)
{
    return (int)gs_clamp(gs_round_to_even((int)ceil(PI0 / acos(1 - gs_min((_MaximumError), (_Radius)) / (_Radius)))), 8, 512);
}

// Immediate2DRenderer
Immediate2DRenderer::Immediate2DRenderer(){}
Immediate2DRenderer::~Immediate2DRenderer(){}

gs_vec3f Immediate2DRenderer::get_cursor_postion() const
{
    gs_vec2f size   = Frenchie::Application::application()->get_window_size();
    gs_vec2f cursor = Frenchie::Application::application()->get_window_cursor_position();
    gs_mat4f matrix =
        gs_matrix_invert_square(m_RenderingQueue->get_cameraview_matrix()) *
        gs_matrix_invert_square(m_RenderingQueue->get_projection_matrix());
    
    return matrix * gs_vec4f(gs_vector_convert_to_NDC(cursor, size), -1.f, 1.f);
}

RenderingQueueMetrics Immediate2DRenderer::get_rendering_queue_metrics() const
{
    return m_RenderingQueue != nullptr ? m_RenderingQueue->get_rendering_queue_metrics() : RenderingQueueMetrics();
}

float Immediate2DRenderer::get_near_plane() const
{
    return -10000.f;
}

float Immediate2DRenderer::get_far_plane() const
{
    return +10000.f;
}

bool Immediate2DRenderer::awake()
{
    if(m_RenderingQueue == nullptr)
       m_RenderingQueue = Frenchie::Application::application()->push_layer<RenderingQueue>();

    return m_RenderingQueue != nullptr;
}

void Immediate2DRenderer::frame_start()
{
    // push clear colo
    push_clear_color(gs_vec4f(150.f, 150.f, 150.f, 150.f));
    push_clip_box(gs_2dboxf(gs_vec2f(0.f, 0.f), application()->get_window_size()));

    // compute projection matrix
    float width  = Frenchie::Application::application()->get_window_size().x;
    float height = Frenchie::Application::application()->get_window_size().y;

    auto camera = calculate_2d_camera_view_and_projection(
        gs_vec2f(width * 0.5f, height * 0.5f),
        gs_vec3f(0.f, 1.f, 0.f),
        gs_vec3f(0.f, 0.f, -1.f),
        gs_vec2f(width, height),
        0.f,
        get_near_plane(),
        get_far_plane()
    );

    gs_mat4f cameraview = camera.cameraview;
    gs_mat4f projection = camera.projection;

    m_RenderingQueue->set_cameraview_matrix(cameraview);
    m_RenderingQueue->set_projection_matrix(projection);

    // compute viewport
    gs_vec3f viewportMin = gs_vector_convert_to_NDC(gs_vec2f(0.f, 0.f), gs_vec2f(width, height));
    gs_vec3f viewportMax = gs_vector_convert_to_NDC(Frenchie::Application::application()->get_window_size(), gs_vec2f(width, height));

    m_RenderingQueueViewport = gs_2dboxf(
        gs_matrix_invert_square(projection) * gs_matrix_invert_square(cameraview) * gs_vec4f(viewportMin, 1.f),
        gs_matrix_invert_square(projection) * gs_matrix_invert_square(cameraview) * gs_vec4f(viewportMax, 1.f));

    pop_clip_box();
}

void Immediate2DRenderer::frame_finish()
{
    // clear rendering queue data
    m_RenderingQueueClearColors.clear();
    m_RenderingQueueClippingBoxes.clear();
    m_RenderingQueueMeshVertexes.clear();
    m_RenderingQueueMeshVertexesIndexes.clear();
}

void Immediate2DRenderer::finish(){}

void Immediate2DRenderer::push_rendering_command(
    const RenderingQueueTexture&            _Texture,
    const RenderingQueueColor&              _Color,
    const gs_mat4f&                         _Transform,
    const RenderingQueueMeshRenderingHints& _MeshRenderingHints)
{
    if(m_RenderingQueueMeshVertexesIndexes.empty() || m_RenderingQueueMeshVertexes.empty()) return;

    // push rendering command
    m_RenderingQueue->push_rendering_command(
        // construct mesh
        m_RenderingQueue->construct_mesh(
            &m_RenderingQueueMeshVertexes[0],
            (int)m_RenderingQueueMeshVertexes.size(),
            &m_RenderingQueueMeshVertexesIndexes[0],
            (int)m_RenderingQueueMeshVertexesIndexes.size()),
        
        // provide default shader
        m_RenderingQueue->get_default_shader(),

        // setup texture
        RenderingQueueTexture(
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

    // clean-up

    // clear rendering queue data
    m_RenderingQueueMeshVertexes.clear();
    m_RenderingQueueMeshVertexesIndexes.clear();
}

void Immediate2DRenderer::push_rendering_command(
    const gs_mat4f&                         _Transform,
    const RenderingQueueMeshRenderingHints& _MeshRenderingHints)
{
    push_rendering_command(m_RenderingQueue->get_default_texture(), 1, _Transform);
}


void Immediate2DRenderer::push_clip_box(const gs_2dboxf& _Value, const gs_mat4f& _Transform)
{
    gs_2dboxf clipRect = _Value.transform(_Transform);
    m_RenderingQueueClippingBoxes.push_back(clipRect);
}

void Immediate2DRenderer::pop_clip_box()
{
    if(!m_RenderingQueueClippingBoxes.empty())
        m_RenderingQueueClippingBoxes.pop_back();
}

void Immediate2DRenderer::push_clear_color(const gs_vec4f& _Value)
{
    m_RenderingQueueClearColors.push_back(_Value);
}

void Immediate2DRenderer::pop_clear_color()
{
    if(!m_RenderingQueueClearColors.empty())
        m_RenderingQueueClearColors.pop_back();
}

gs_2dboxf Immediate2DRenderer::current_clipping_box() const
{
    return !m_RenderingQueueClippingBoxes.empty() ? m_RenderingQueueClippingBoxes[m_RenderingQueueClippingBoxes.size() - 1] : gs_2dboxf(gs_vec2f(0.f, 0.f), application()->get_window_size());
}

gs_vec4f Immediate2DRenderer::current_clear_color() const
{
    return !m_RenderingQueueClearColors.empty() ? m_RenderingQueueClearColors[m_RenderingQueueClearColors.size() - 1] : gs_vec4f(255.f, 255.f, 255.f, 255.f);
}

gs_mat4f Immediate2DRenderer::calculate_transform_matrix(
    const float&    _Depth,
    const gs_vec2f& _Position,
    const float&    _Rotation,
    const gs_vec2f& _Scale)
{
    gs_mat4f matrix(1.f);

    return gs_matrix_translate(matrix, gs_vec3f(_Position, _Depth)) *
            gs_matrix_rotate(matrix, gs_to_radians(_Rotation), gs_vec3f(0.f, 0.f, 1.f)) * 
            gs_matrix_scale(matrix, gs_vec3f(_Scale, 1.f));
}

gs_vec2f Immediate2DRenderer::calculate_arc_point(
    const gs_vec2f& _Center,
    const float&    _MinorRadius,
    const float&    _MajorRadius,
    const float&    _ArcAngle)
{
    return gs_vec2f(
        _Center.x + _MinorRadius * cos(gs_to_radians(_ArcAngle)),
        _Center.y + _MajorRadius * sin(gs_to_radians(_ArcAngle)));
}

gs_2dboxf Immediate2DRenderer::calculate_bounding_box(
    const std::u32string&     _Text,
    const float&              _Size,
    const RenderingQueueFont& _Font)
{
    RenderingQueueFont font = _Font.is_null() ? m_RenderingQueue->get_default_font() : _Font;

    float scale     = _Size / (float)font.SizeInPixels;
    float offset    = (font.Ascent + font.Descent + font.LineGap) * scale;
    float positionX = 0.f;
    float positionY = gs_vec2f(0.f, offset).y;

    gs_vec2f min;
    gs_vec2f max;

    for(int i = 0; i < (int)_Text.size(); ++i)
    {
        unsigned int symbol = _Text[i];

        // fallbacks
        if(!font.contains_glyph(symbol))
        {
            // next line
            if(symbol == '\n')
            {
                positionY += gs_vec2f(0.f, gs_max(_Size, gs_abs(offset))).y;
                positionX =  0.f;
            }
            // carriage return
            else if(symbol == '\r')
                positionX =  0.f;
            // tab
            else if(symbol == '\t')
                positionX += gs_vec2f(_Size, 0.f).x;
            else
            {
                // TODO: do someting here...
                // May be use fallback font and take fallback character from there ???
            }

            continue;
        }

        min = gs_vec2f(gs_min(positionX, min.x, max.x), gs_min(positionY, min.y, max.y));
        max = gs_vec2f(gs_max(positionX, min.x, max.x), gs_max(positionY, min.y, max.y));

        // move cursor
        positionX += gs_vec2f(font.retrieve_glyph(symbol).Advance * scale, 0.f).x;
    }

    min = gs_vec2f(gs_min(positionX, min.x, max.x), gs_min(positionY, min.y, max.y));
    max = gs_vec2f(gs_max(positionX, min.x, max.x), gs_max(positionY, min.y, max.y));

    return gs_2dboxf(min, max);
}

gs_2dboxf Immediate2DRenderer::calculate_bounding_box(
    const std::u16string&     _Text,
    const float&              _Size,
    const RenderingQueueFont& _Font)
{
    return calculate_bounding_box(
        Frenchie::Core::String::convert_utf16_to_utf8(_Text),
        _Size,
        _Font
    );
}

gs_2dboxf Immediate2DRenderer::calculate_bounding_box(
    const std::string&        _Text,
    const float&              _Size,
    const RenderingQueueFont& _Font)
{
    return calculate_bounding_box(
        Frenchie::Core::String::convert_utf8_to_utf32(_Text),
        _Size,
        _Font);
}

void Immediate2DRenderer::push_triangle_filled(
    const gs_vec2f&              _P1,
    const gs_vec2f&              _P2,
    const gs_vec2f&              _P3,
    const RenderingQueueColor&   _Color,
    const gs_mat4f&              _Transform,
    const RenderingQueueTexture& _Texture)
{
    if( !current_clipping_box().contains(_Transform * gs_vec4f(_P1, 0.f, 1.f)) &&
        !current_clipping_box().contains(_Transform * gs_vec4f(_P2, 0.f, 1.f)) &&
        !current_clipping_box().contains(_Transform * gs_vec4f(_P3, 0.f, 1.f)))
    {
        return;
    }

    build_triangle_filled_mesh(_P1, _P2, _P3, _Color, !_Texture.is_null() ? _Texture : m_RenderingQueue->get_default_texture());
    push_rendering_command(!_Texture.is_null() ? _Texture : m_RenderingQueue->get_default_texture(), _Color, _Transform);
}

void Immediate2DRenderer::push_rectangle_filled(
    const gs_vec2f&              _Min,
    const gs_vec2f&              _Max,
    const RenderingQueueColor&   _Color,
    const gs_mat4f&              _Transform,
    const RenderingQueueTexture& _Texture)
{
    if(!current_clipping_box().overlaps(gs_2dboxf(_Transform * gs_vec4f(_Min, 0.f, 1.f), _Transform * gs_vec4f(_Max, 0.f, 1.f))))
        return;

    build_rectangle_filled_mesh(_Min, _Max, _Color, !_Texture.is_null() ? _Texture : m_RenderingQueue->get_default_texture());
    push_rendering_command(!_Texture.is_null() ? _Texture : m_RenderingQueue->get_default_texture(), _Color, _Transform);
}

void Immediate2DRenderer::push_rectangle_rounded_filled(
    const gs_vec2f&            _Min,
    const gs_vec2f&            _Max,
    const float&               _Radius,
    const RenderingQueueColor& _Color,
    const gs_mat4f&            _Transform)
{
    if(!current_clipping_box().overlaps(gs_2dboxf(_Transform * gs_vec4f(_Min, 0.f, 1.f), _Transform * gs_vec4f(_Max, 0.f, 1.f))))
        return;

    // auxiliary lambdas
    auto build_rectangle_rounded_filled_mesh = [this](
        const gs_vec2f&            _Min,
        const gs_vec2f&            _Max,
        const float&               _Radius,
        const RenderingQueueColor& _Color)
    {
        // compute radius
        float radius = gs_min(gs_min(_Radius, gs_abs(_Max.x - _Min.x) * 0.5f), gs_min(_Radius, gs_abs(_Max.y - _Min.y) * 0.5f));

        // build angle ellipses
        const gs_vec2f p1 = gs_vec2f(_Min.x, _Min.y);
        const gs_vec2f p2 = gs_vec2f(_Max.x, _Min.y);
        const gs_vec2f p3 = gs_vec2f(_Max.x, _Max.y);
        const gs_vec2f p4 = gs_vec2f(_Min.x, _Max.y);

        gs_vec2f p13 = gs_vector_normalize(p1 - p3);
        gs_vec2f p24 = gs_vector_normalize(p2 - p4);
        p13 = gs_vec2f(gs_sign(p13.x), gs_sign(p13.y));
        p24 = gs_vec2f(gs_sign(p24.x), gs_sign(p24.y));

        build_arc_filled_mesh(p1 - p13 * radius, radius, radius, 0.f, 360.f, _Color, m_RenderingQueue->get_default_texture(), 16);
        build_arc_filled_mesh(p2 - p24 * radius, radius, radius, 0.f, 360.f, _Color, m_RenderingQueue->get_default_texture(), 16);
        build_arc_filled_mesh(p3 + p13 * radius, radius, radius, 0.f, 360.f, _Color, m_RenderingQueue->get_default_texture(), 16);
        build_arc_filled_mesh(p4 + p24 * radius, radius, radius, 0.f, 360.f, _Color, m_RenderingQueue->get_default_texture(), 16);

        // build inner rectnagle
        Immediate2DRenderer::build_rectangle_filled_mesh(
            p1 - p13 * gs_vec2f(radius, 0.f),
            p3 + p13 * gs_vec2f(radius, 0.f),
            _Color,
            m_RenderingQueue->get_default_texture());

        Immediate2DRenderer::build_rectangle_filled_mesh(
            p1 - p13 * gs_vec2f(0.f, radius),
            p3 + p13 * gs_vec2f(0.f, radius),
            _Color,
            m_RenderingQueue->get_default_texture());
    };

    build_rectangle_rounded_filled_mesh(_Min, _Max, _Radius, _Color);
    push_rendering_command(m_RenderingQueue->get_default_texture(), _Color, _Transform);
}

void Immediate2DRenderer::push_text(
    const std::u32string&      _Text,
    const float&               _Size,
    const RenderingQueueColor& _Color,
    const gs_mat4f&            _Transform,
    const RenderingQueueFont&  _Font)
{
    RenderingQueueFont font = _Font.is_null() ? m_RenderingQueue->get_default_font() : _Font;

    float scale     = _Size / (float)font.SizeInPixels;
    float offset    = (font.Ascent + font.Descent + font.LineGap) * scale;
    float positionX = 0.f;
    float positionY = gs_vec2f(0.f, offset).y;

    for(int i = 0; i < (int)_Text.size(); ++i)
    {
        unsigned int symbol = _Text[i];

        // fallbacks
        if(!font.contains_glyph(symbol))
        {
            // next line
            if(symbol == '\n')
            {
                positionY += gs_vec2f(0.f, gs_max(_Size, gs_abs(offset))).y;
                positionX =  0.f;
            }
            // carriage return
            else if(symbol == '\r')
                positionX =  0.f;
            // tab
            else if(symbol == '\t')
                positionX += gs_vec2f(_Size, 0.f).x;
            else
            {
                // TODO: do someting here...
                // May be use fallback font and take fallback character from there ???
            }

            continue;
        }

        RenderingQueueGlyph glyph    = font.retrieve_glyph(symbol);
        float glyphWidth             = glyph.Box.size().x * scale;
        float glyphHeight            = glyph.Box.size().y * scale;
        float glyphHorizontalBearing = glyph.Bearing.x * scale;
        float glyphVerticalBearing   = glyph.Bearing.y * scale;
        float glyphAdvance           = glyph.Advance * scale;

        auto min = gs_vec2f(positionX, positionY) + gs_vec2f(glyphHorizontalBearing, glyphVerticalBearing);
        auto max = min + gs_vec2f(glyphWidth, glyphHeight);

        build_rectangle_filled_mesh(
            min,
            max,
            glyph.MinUV,
            glyph.MaxUV,
            _Color);

        // move cursor
        positionX += gs_vec2f(glyphAdvance, 0.f).x;
    }

    push_rendering_command(font.AtlasTexture, _Color, _Transform);
}

void Immediate2DRenderer::push_text(
    const std::u16string&      _Text,
    const float&               _Size,
    const RenderingQueueColor& _Color,
    const gs_mat4f&            _Transform,
    const RenderingQueueFont&  _Font)
{
    push_text(
        Frenchie::Core::String::convert_utf16_to_utf8(_Text),
        _Size,
        _Color,
        _Transform,
        _Font
    );
}

void Immediate2DRenderer::push_text(
    const std::string&         _Text,
    const float&               _Size,
    const RenderingQueueColor& _Color,
    const gs_mat4f&            _Transform,
    const RenderingQueueFont&  _Font)
{
    push_text(
        Frenchie::Core::String::convert_utf8_to_utf32(_Text),
        _Size,
        _Color,
        _Transform,
        _Font
    );
}

void Immediate2DRenderer::push_arc_filled(
    const gs_vec2f&              _Center,
    const float&                 _MinorRadius,
    const float&                 _MajorRadius,
    const float&                 _SourceAngle,
    const float&                 _TargetAngle,
    const RenderingQueueColor& _Color,
    const gs_mat4f&              _Transform,
    const RenderingQueueTexture& _Texture)
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
        !_Texture.is_null() ? _Texture : m_RenderingQueue->get_default_texture());

    push_rendering_command(
        !_Texture.is_null() ? _Texture : m_RenderingQueue->get_default_texture(),
        _Color,
        _Transform);
}

void Immediate2DRenderer::push_line(
    const gs_vec2f&            _P1,
    const gs_vec2f&            _P2,
    const float&               _Width,
    const RenderingQueueColor& _Color,
    const gs_mat4f&            _Transform)
{
    if(!current_clipping_box().contains(_Transform * gs_vec4f(_P1, 0.f, 1.f)) &&
        !current_clipping_box().contains(_Transform * gs_vec4f(_P2, 0.f, 1.f)))
    {
        return;
    }

    Immediate2DRenderer::build_line_mesh(
        _P1,
        _P2,
        _Width,
        _Color,
        m_RenderingQueue->get_default_texture());

    push_rendering_command(m_RenderingQueue->get_default_texture(), _Color, _Transform);
}

void Immediate2DRenderer::push_arc(
    const gs_vec2f&            _Center,
    const float&               _MinorRadius,
    const float&               _MajorRadius,
    const float&               _SourceAngle,
    const float&               _TargetAngle,
    const float&               _Width,
    const RenderingQueueColor& _Color,
    const gs_mat4f&            _Transform)
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
    push_rendering_command(m_RenderingQueue->get_default_texture(), _Color, _Transform);
}

void Immediate2DRenderer::push_triangle(
    const gs_vec2f&            _P1,
    const gs_vec2f&            _P2,
    const gs_vec2f&            _P3,
    const float&               _Width,
    const RenderingQueueColor& _Color,
    const gs_mat4f&            _Transform)
{
    // check if we are within viewport
    if( !current_clipping_box().contains(_Transform * gs_vec4f(_P1, 0.f, 1.f)) &&
        !current_clipping_box().contains(_Transform * gs_vec4f(_P2, 0.f, 1.f)) &&
        !current_clipping_box().contains(_Transform * gs_vec4f(_P3, 0.f, 1.f)))
    {
        return;
    }

    // build path
    m_PathBuilder.begin(_P1);
    m_PathBuilder.line_to(_P2);
    m_PathBuilder.line_to(_P3);

    // build mesh
    m_PathBuilder.build_mesh(
        _Color,
        m_RenderingQueue->get_default_texture(),
        _Width,
        m_RenderingQueueMeshVertexes,
        m_RenderingQueueMeshVertexesIndexes);

    // push rendering command
    push_rendering_command(_Transform, _Color);
}

void Immediate2DRenderer::push_rectangle(
    const gs_vec2f&            _Min,
    const gs_vec2f&            _Max,
    const float&               _Width,
    const RenderingQueueColor& _Color,
    const gs_mat4f&            _Transform)
{
    if(!current_clipping_box().overlaps(
        gs_2dboxf(
            _Transform * gs_vec4f(_Min, 0.f, 1.f, 1.f),
            _Transform * gs_vec4f(_Max, 0.f, 1.f, 1.f))))
    {
        return;
    }

    // build path
    m_PathBuilder.begin(_Min);
    m_PathBuilder.line_to(gs_vec2f(_Max.x, _Min.y));
    m_PathBuilder.line_to(_Max);
    m_PathBuilder.line_to(gs_vec2f(_Min.x, _Max.y));

    // build mesh
    m_PathBuilder.build_mesh(
        _Color,
        m_RenderingQueue->get_default_texture(),
        _Width,
        m_RenderingQueueMeshVertexes,
        m_RenderingQueueMeshVertexesIndexes);

    // push rendering command
    push_rendering_command(_Transform, _Color);
}

void Immediate2DRenderer::push_rectangle_rounded(
    const gs_vec2f&            _Min,
    const gs_vec2f&            _Max,
    const float&               _Radius,
    const float&               _Width,
    const RenderingQueueColor& _Color,
    const gs_mat4f&            _Transform)
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
        push_rectangle(_Min, _Max, _Width, _Color);
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
    build_line_mesh(calculate_arc_point(TL, radius, radius, 180), calculate_arc_point(BL, radius, radius, 180), _Width, _Color, m_RenderingQueue->get_default_texture());
    build_line_mesh(calculate_arc_point(TL, radius, radius, 90), calculate_arc_point(TR, radius, radius, 90), _Width, _Color, m_RenderingQueue->get_default_texture());
    build_line_mesh(calculate_arc_point(TR, radius, radius, 0), calculate_arc_point(BR, radius, radius, 0), _Width, _Color, m_RenderingQueue->get_default_texture());
    build_line_mesh(calculate_arc_point(BL, radius, radius, 270), calculate_arc_point(BR, radius, radius, 270), _Width, _Color, m_RenderingQueue->get_default_texture());

    push_rendering_command(m_RenderingQueue->get_default_texture(), _Color, _Transform);
}

void Immediate2DRenderer::build_triangle_filled_mesh(
    const gs_vec2f&                    _P1,
    const gs_vec2f&                    _P2,
    const gs_vec2f&                    _P3,
    const RenderingQueueColor&         _Color,
    const RenderingQueueTexture&       _Texture)
{
    const int size = (int)m_RenderingQueueMeshVertexes.size();

    m_RenderingQueueMeshVertexes.push_back(
        RenderingQueue::construct_vertex(
            gs_vec3f(_P1.x, _P1.y, 0.f),
            gs_vec3f(0.f), gs_vec2f(_P1.x / _Texture.Width, _P1.y / _Texture.Height),
            _Color));
    
    m_RenderingQueueMeshVertexes.push_back(
        RenderingQueue::construct_vertex(
            gs_vec3f(_P2.x, _P2.y, 0.f),
            gs_vec3f(0.f), gs_vec2f(_P2.x / _Texture.Width, _P2.y / _Texture.Height),
            _Color));
    
    m_RenderingQueueMeshVertexes.push_back(
        RenderingQueue::construct_vertex(
            gs_vec3f(_P3.x, _P3.y, 0.f),
            gs_vec3f(0.f),
            gs_vec2f(_P3.x / _Texture.Width, _P3.y / _Texture.Height),
            _Color));
    
    for (int i = size; i < (int)m_RenderingQueueMeshVertexes.size(); ++i)
        m_RenderingQueueMeshVertexesIndexes.push_back(i);
}

void Immediate2DRenderer::build_rectangle_filled_mesh(
    const gs_vec2f&              _Min,
    const gs_vec2f&              _Max,
    const RenderingQueueColor&   _Color,
    const RenderingQueueTexture& _Texture)
{
    const int      size   = (int)m_RenderingQueueMeshVertexes.size();
    const gs_vec3f point1 = gs_vec3f(_Min.x, _Min.y, 0.f);
    const gs_vec3f point2 = gs_vec3f(_Max.x, _Min.y, 0.f);
    const gs_vec3f point3 = gs_vec3f(_Max.x, _Max.y, 0.f);
    const gs_vec3f point4 = gs_vec3f(_Min.x, _Max.y, 0.f);

    // triangle 1
    m_RenderingQueueMeshVertexes.push_back(
        RenderingQueue::construct_vertex(
            point1,
            gs_vec3f(0.f),
            gs_vec2f(point1.x / _Texture.Width, point1.y / _Texture.Height),
            _Color));

    m_RenderingQueueMeshVertexes.push_back(
        RenderingQueue::construct_vertex(
            point2,
            gs_vec3f(0.f),
            gs_vec2f(point2.x / _Texture.Width, point2.y / _Texture.Height),
            _Color));

    m_RenderingQueueMeshVertexes.push_back(
        RenderingQueue::construct_vertex(
            point4,
            gs_vec3f(0.f),
            gs_vec2f(point4.x / _Texture.Width, point4.y / _Texture.Height),
            _Color));

    // triangle 2
    m_RenderingQueueMeshVertexes.push_back(
        RenderingQueue::construct_vertex(
            point2,
            gs_vec3f(0.f),
            gs_vec2f(point2.x / _Texture.Width, point2.y / _Texture.Height),
            _Color));

    m_RenderingQueueMeshVertexes.push_back(
        RenderingQueue::construct_vertex(
            point3, gs_vec3f(0.f),
            gs_vec2f(point3.x / _Texture.Width, point3.y / _Texture.Height),
            _Color));

    m_RenderingQueueMeshVertexes.push_back(
        RenderingQueue::construct_vertex(
            point4,
            gs_vec3f(0.f),
            gs_vec2f(point4.x / _Texture.Width, point4.y / _Texture.Height),
            _Color));

    for (int i = size; i < (int)m_RenderingQueueMeshVertexes.size(); ++i)
        m_RenderingQueueMeshVertexesIndexes.push_back(i);
}

void Immediate2DRenderer::build_rectangle_filled_mesh(
    const gs_vec2f&            _Min,
    const gs_vec2f&            _Max,
    const gs_vec2f&            _MinUV,
    const gs_vec2f&            _MaxUV,
    const RenderingQueueColor& _Color)
{
    const int size = (int)m_RenderingQueueMeshVertexes.size();

    const gs_vec3f _P1 = gs_vec3f(_Min.x, _Min.y, 0.f);
    const gs_vec3f _P2 = gs_vec3f(_Max.x, _Min.y, 0.f);
    const gs_vec3f _P3 = gs_vec3f(_Max.x, _Max.y, 0.f);
    const gs_vec3f _P4 = gs_vec3f(_Min.x, _Max.y, 0.f);

    const gs_vec3f _UV1 = gs_vec3f(_MinUV.x, _MinUV.y, 0.f);
    const gs_vec3f _UV2 = gs_vec3f(_MaxUV.x, _MinUV.y, 0.f);
    const gs_vec3f _UV3 = gs_vec3f(_MaxUV.x, _MaxUV.y, 0.f);
    const gs_vec3f _UV4 = gs_vec3f(_MinUV.x, _MaxUV.y, 0.f);

    // triangle 1
    m_RenderingQueueMeshVertexes.push_back(
        RenderingQueue::construct_vertex(
            _P1,
            gs_vec3f(0.f),
            gs_vec2f(_UV1.x, _UV1.y),
            _Color));

    m_RenderingQueueMeshVertexes.push_back(
        RenderingQueue::construct_vertex(
            _P2,
            gs_vec3f(0.f),
            gs_vec2f(_UV2.x, _UV2.y),
            _Color));

    m_RenderingQueueMeshVertexes.push_back(
        RenderingQueue::construct_vertex(
            _P4,
            gs_vec3f(0.f),
            gs_vec2f(_UV4.x, _UV4.y),
            _Color));

    // triangle 2
    m_RenderingQueueMeshVertexes.push_back(
        RenderingQueue::construct_vertex(
            _P2,
            gs_vec3f(0.f),
            gs_vec2f(_UV2.x, _UV2.y),
            _Color));

    m_RenderingQueueMeshVertexes.push_back(
        RenderingQueue::construct_vertex(
            _P3,
            gs_vec3f(0.f),
            gs_vec2f(_UV3.x, _UV3.y),
            _Color));

    m_RenderingQueueMeshVertexes.push_back(
        RenderingQueue::construct_vertex(
            _P4,
            gs_vec3f(0.f),
            gs_vec2f(_UV4.x, _UV4.y),
            _Color));

    for (int i = size; i < (int)m_RenderingQueueMeshVertexes.size(); ++i)
        m_RenderingQueueMeshVertexesIndexes.push_back(i);
}

void Immediate2DRenderer::build_arc_filled_mesh(
    const gs_vec2f&              _Center,
    const float&                 _MinorRadius,
    const float&                 _MajorRadius,
    const float&                 _SourceAngle,
    const float&                 _TargetAngle,
    const RenderingQueueColor&   _Color,
    const RenderingQueueTexture& _Texture,
    const int&                   _SegmentsCount)
{
    gs_vec2f p0 = gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(_SourceAngle)), _Center.y + _MajorRadius * sin(gs_to_radians(_SourceAngle)));
    gs_vec2f p1 = p0;
    gs_vec2f p2 = p0;

    const float angleIncrement = (_TargetAngle - _SourceAngle) / _SegmentsCount;

    for (float angle = _SourceAngle; angle <= _TargetAngle; angle += angleIncrement, p1 = p2)
    {
        p2 = gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(angle)), _Center.y + _MajorRadius * sin(gs_to_radians(angle)));
        build_triangle_filled_mesh(_Center, p1, p2, _Color, _Texture);
    }
}

void Immediate2DRenderer::build_line_mesh(
    const gs_vec2f&              _P1,
    const gs_vec2f&              _P2,
    const float&                 _Width,
    const RenderingQueueColor&   _Color,
    const RenderingQueueTexture& _Texture)
{
    const gs_vec3f p1 = gs_vec3f(_P1.x, _P1.y, 0.f);
    const gs_vec3f p2 = gs_vec3f(_P2.x, _P2.y, 0.f);

    gs_vec3f direction     = gs_vector_normalize(_P2 - _P1);
    gs_vec2f perpendicular = gs_vector_normalize(gs_vector_cross(direction, gs_vec3f(0.f, 0.f, 1.f))) * _Width * 0.5f;

    build_triangle_filled_mesh(
        _P1 - perpendicular,
        _P2 - perpendicular,
        _P1 + perpendicular,
        _Color,
        _Texture);

    build_triangle_filled_mesh(
        _P1 + perpendicular,
        _P2 - perpendicular,
        _P2 + perpendicular,
        _Color,
        _Texture);
}

void Immediate2DRenderer::build_arc_mesh(
    const gs_vec2f&            _Center,
    const float&               _MinorRadius,
    const float&               _MajorRadius,
    const float&               _SourceAngle,
    const float&               _TargetAngle,
    const float&               _Width,
    const RenderingQueueColor& _Color)
{
    const float angleIncrement = (_TargetAngle - _SourceAngle) / 36.f;

    gs_vec2f p1 = gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(_SourceAngle)), _Center.y + _MajorRadius * sin(gs_to_radians(_SourceAngle)));
    gs_vec2f p2;
    
    for (float angle = _SourceAngle; angle <= _TargetAngle; angle += angleIncrement, p1 = p2)
    {
        p2 = gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(angle)), _Center.y + _MajorRadius * sin(gs_to_radians(angle)));
        build_line_mesh(p1, p2, _Width, _Color, m_RenderingQueue->get_default_texture());
    }
}