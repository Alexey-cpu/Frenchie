#include <FrenchieApplicationImmediate2DRenderer.hpp>

// Core
#include <FrenchieCoreStringUtilities.hpp>

using namespace Frenchie::Application;

// Immediate2DRenderer
Immediate2DRenderer::Immediate2DRenderer(){}
Immediate2DRenderer::~Immediate2DRenderer(){}

bool Immediate2DRenderer::awake()
{
    if(m_RenderingQueue == nullptr)
       m_RenderingQueue = Frenchie::Application::application()->push_layer<RenderingQueue>();

    return m_RenderingQueue != nullptr;
}

// auto calculate_orthographic_camera_view_and_projection_matrix(
//     const gs_vec3f& _CameraWorldPosition,
//     const gs_vec3f& _CameraWorldUpAxisDirection,
//     const gs_vec2f& _CameraResolution,
//     const gs_vec4f& _CameraEulerAngles,
//     const float&    _CameraNearPlanePosition,
//     const float&    _CameraFarPlanePosition)
// {
//     // compute projection matrix
//     float width  = _CameraResolution.x;
//     float height = _CameraResolution.y;

//     float left   = -width  * 0.5f + _CameraWorldPosition.x; // The x-coordinate of the left edge of the viewable area.
//     float right  = +width  * 0.5f + _CameraWorldPosition.x; // The x-coordinate of the right edge of the viewable area.
//     float bottom = -height * 0.5f + _CameraWorldPosition.y; // The y-coordinate of the bottom edge of the viewable area.
//     float top    = +height * 0.5f + _CameraWorldPosition.y; // The y-coordinate of the top edge of the viewable area.

//     // float left   = -width  * 0.5f + width  * 0.5f; // The x-coordinate of the left edge of the viewable area.
//     // float right  = +width  * 0.5f + width  * 0.5f; // The x-coordinate of the right edge of the viewable area.
//     // float bottom = -height * 0.5f - height * 0.5f; // The y-coordinate of the bottom edge of the viewable area.
//     // float top    = +height * 0.5f - height * 0.5f; // The y-coordinate of the top edge of the viewable area.

//     // camera rotation angles
//     gs_mat4f rotateX = gs_matrix_rotate(gs_mat4f(1.f), gs_to_radians(_CameraEulerAngles.x), gs_vec3f(1.f, 0.f, 0.f));
//     gs_mat4f rotateY = gs_matrix_rotate(gs_mat4f(1.f), gs_to_radians(_CameraEulerAngles.y), gs_vec3f(0.f, 1.f, 0.f));
//     gs_mat4f rotateZ = gs_matrix_rotate(gs_mat4f(1.f), gs_to_radians(_CameraEulerAngles.z), gs_vec3f(0.f, 0.f, 1.f));

//     // camera orientation
//     gs_vec3f cameraLocalFrontAxisDirection = gs_vector_normalize(rotateY * rotateX * gs_vec4f(gs_vec3f(0.f, 0.f, -1), 1.f));//gs_vector_normalize(gs_vec3f(0.f, 0.f, -1.f));
//     gs_vec3f cameraLocalRightAxisDirection = gs_vector_normalize(gs_vector_cross(cameraLocalFrontAxisDirection, _CameraWorldUpAxisDirection));
//     gs_vec3f cameraLocalUpAxisDirection    = gs_vector_normalize(gs_vector_cross(cameraLocalRightAxisDirection, cameraLocalFrontAxisDirection));

//     // rotate around Z axis
//     cameraLocalFrontAxisDirection = gs_vector_normalize(gs_vec3f(rotateZ * gs_vec4f(cameraLocalFrontAxisDirection, 1.f)));
//     cameraLocalUpAxisDirection    = gs_vector_normalize(gs_vec3f(rotateZ * gs_vec4f(_CameraWorldUpAxisDirection, 1.f)));

//     gs_vec3f pos = {0.f, 0.f, 1.f};

//     struct
//     {
//         gs_mat4f cameraview;
//         gs_mat4f projection;
//     } result = {
//         gs_matrix_look_at(pos, pos + cameraLocalFrontAxisDirection, cameraLocalUpAxisDirection),
//         gs_matrix_ortho(left, right, bottom, top, _CameraNearPlanePosition, _CameraFarPlanePosition)
//     };

//     return result;
// }

void Immediate2DRenderer::frame_start()
{
    // compute projection matrix
    float width  = Frenchie::Application::application()->get_window_size().x;
    float height = Frenchie::Application::application()->get_window_size().y;

    auto camera = gs_matrix_calculate_perspective_camera_view_and_projection(
        gs_vec3f(width * 0.5f, -height * 0.5f, 5000.f),
        gs_vec3f(0.f, 1.f, 0.f),
        gs_vec3f(0.f, 0.f, -1.f),
        gs_vec2f(width, height),
        gs_vec3f(0.f, 0.f, 0.f),
        +500.f,
        -1000.f
    );

    auto cameraview = camera.cameraview;
    auto projection = camera.projection;

    m_RenderingQueue->set_cameraview_matrix(cameraview);
    m_RenderingQueue->set_projection_matrix(projection);

    // compute viewport
    gs_vec3f viewportMin = gs_vector_convert_to_NDC(gs_vec2f(0.f, 0.f), gs_vec2f(width, height));
    gs_vec3f viewportMax = gs_vector_convert_to_NDC(Frenchie::Application::application()->get_window_size(), gs_vec2f(width, height));

    m_Viewport = gs_2dboxf(
        gs_matrix_invert_square(projection) * gs_matrix_invert_square(cameraview) * gs_vec4f(viewportMin, 1.f),
        gs_matrix_invert_square(projection) * gs_matrix_invert_square(cameraview) * gs_vec4f(viewportMax, 1.f)
    );
}

void Immediate2DRenderer::frame_finish()
{
    m_Vertexes.clear();
    m_Indexes.clear();
}

void Immediate2DRenderer::finish(){}

void Immediate2DRenderer::push_rendering_command(const RenderingQueueTexture& _Texture, const gs_vec4f& _Color, const gs_mat4f& _Transform)
{
    if(m_Indexes.empty() || m_Vertexes.empty()) return;

    // push rendering command
    m_RenderingQueue->push_command(
        // construct mesh
        m_RenderingQueue->construct_mesh(
            &m_Vertexes[0],
            (int)m_Vertexes.size(),
            &m_Indexes[0],
            (int)m_Indexes.size()),
        
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
        _Transform);

    // clean-up
    m_Indexes.clear();
    m_Vertexes.clear();
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
    const float&              _Depth,
    const gs_vec2f&           _Position,
    const float&              _Rotation,
    const gs_vec2f&           _Scale,
    const std::u32string&     _Text,
    const float&              _Size,
    const RenderingQueueFont& _Font)
{
    RenderingQueueFont font = _Font.is_null() ? m_RenderingQueue->get_default_font() : _Font;

    float scale     = _Size / (float)font.SizeInPixels;
    float offset    = (font.Ascent + font.Descent - font.LineGap) * scale;
    float positionX = 0.f;
    float positionY = -offset;

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
                positionY -= gs_max(_Size, gs_abs(offset));
                positionX =  0.f;
            }
            // carriage return
            else if(symbol == '\r')
                positionX =  0.f;
            // tab
            else if(symbol == '\t')
                positionX += _Size;
            else
            {
                // TODO: do someting here...
                // May be use fallback font and take fallback character from there ???
            }

            continue;
        }

        RenderingQueueGlyph glyph    = font.retrieve_glyph(symbol);
        float glyphWidth             = glyph.Box.get_size().x * scale;
        float glyphHeight            = glyph.Box.get_size().y * scale;
        float glyphHorizontalBearing = glyph.Bearing.x * scale;
        float glyphVerticalBearing   = glyph.Bearing.y * scale;
        float glyphAdvance           = glyph.Advance * scale;

        min = gs_vec2f(
            gs_min(positionX + glyphHorizontalBearing, min.x),
            gs_min(positionY - glyphVerticalBearing, min.y)
        );

        max = gs_vec2f(
            gs_max(positionX + glyphHorizontalBearing + glyphWidth, max.y),
            gs_max(positionY - glyphVerticalBearing - glyphHeight, max.y)
        );

        // move cursor
        positionX += glyphAdvance;
    }

    auto transform = calculate_transform_matrix(_Depth, _Position, _Rotation, _Scale);

    return gs_2dboxf(
        transform * gs_vec4f(min, _Depth, 1.f),
        transform * gs_vec4f(max, _Depth, 1.f)
    );
}

gs_2dboxf Immediate2DRenderer::calculate_bounding_box(
    const float&              _Depth,
    const gs_vec2f&           _Position,
    const float&              _Rotation,
    const gs_vec2f&           _Scale,
    const std::u16string&     _Text,
    const float&              _Size,
    const RenderingQueueFont& _Font)
{
    return calculate_bounding_box(
        _Depth,
        _Position,
        _Rotation,
        _Scale,
        Frenchie::Core::String::convert_utf16_to_utf8(_Text),
        _Size,
        _Font
    );
}

gs_2dboxf Immediate2DRenderer::calculate_bounding_box(
    const float&              _Depth,
    const gs_vec2f&           _Position,
    const float&              _Rotation,
    const gs_vec2f&           _Scale,
    const std::string&        _Text,
    const float&              _Size,
    const RenderingQueueFont& _Font)
{
    return calculate_bounding_box(
        _Depth,
        _Position,
        _Rotation,
        _Scale,
        Frenchie::Core::String::convert_utf8_to_utf32(_Text),
        _Size,
        _Font
    );
}

void Immediate2DRenderer::push_triangle_filled(
    const gs_vec2f&              _P1,
    const gs_vec2f&              _P2,
    const gs_vec2f&              _P3,
    const gs_vec4f&              _Color,
    const float&                 _Depth,
    const gs_vec2f&              _Position,
    const float&                 _Rotation,
    const gs_vec2f&              _Scale,
    const RenderingQueueTexture& _Texture)
{
    gs_mat4f _Transform =
        calculate_transform_matrix(_Depth, _Position, _Rotation, _Scale);

    if(!m_Viewport.contains(_Transform * gs_vec4f(_P1, _Depth, 1.f)) &&
        !m_Viewport.contains(_Transform * gs_vec4f(_P2, _Depth, 1.f)) &&
        !m_Viewport.contains(_Transform * gs_vec4f(_P3, _Depth, 1.f)))
    {
        return;
    }

    Immediate2DRenderer::build_triangle_filled_mesh(
        _P1,
        _P2,
        _P3,
        _Color,
        !_Texture.is_null() ? _Texture : m_RenderingQueue->get_default_texture(),
        m_Vertexes,
        m_Indexes);

    push_rendering_command(
        !_Texture.is_null() ? _Texture : m_RenderingQueue->get_default_texture(),
        _Color,
        _Transform
    );
}

void Immediate2DRenderer::push_rectangle_filled(
    const gs_vec2f&              _Min,
    const gs_vec2f&              _Max,
    const gs_vec4f&              _Color,
    const float&                 _Depth,
    const gs_vec2f&              _Position,
    const float&                 _Rotation,
    const gs_vec2f&              _Scale,
    const RenderingQueueTexture& _Texture)
{
    gs_mat4f _Transform =
        calculate_transform_matrix(_Depth, _Position, _Rotation, _Scale);

    if(!m_Viewport.overlaps(
        gs_2dboxf(
            _Transform * gs_vec4f(_Min, _Depth, 1.f),
            _Transform * gs_vec4f(_Max, _Depth, 1.f))))
    {
        return;
    }

    Immediate2DRenderer::build_rectangle_filled_mesh(
        _Min,
        _Max,
        _Color,
        !_Texture.is_null() ? _Texture : m_RenderingQueue->get_default_texture(),
        m_Vertexes,
        m_Indexes);

    push_rendering_command(
        !_Texture.is_null() ? _Texture : m_RenderingQueue->get_default_texture(),
        _Color,
        _Transform);
}

void Immediate2DRenderer::push_rectangle_rounded_filled(
    const gs_vec2f& _Min,
    const gs_vec2f& _Max,
    const float&    _Radius,
    const gs_vec4f& _Color,
    const float&    _Depth,
    const gs_vec2f& _Position,
    const float&    _Rotation,
    const gs_vec2f& _Scale)
{
    gs_mat4f _Transform =
        calculate_transform_matrix(_Depth, _Position, _Rotation, _Scale);

    // check that we are within viewport
    if(!m_Viewport.overlaps(
        gs_2dboxf(
            _Transform * gs_vec4f(_Min, _Depth, 1.f, 1.f),
            _Transform * gs_vec4f(_Max, _Depth, 1.f, 1.f))))
    {
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
    build_arc_filled_mesh(TL, radius, radius, 90.f, 180.f, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);
    build_arc_filled_mesh(BL, radius, radius, 180.f, 270.f, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);
    build_arc_filled_mesh(TR, radius, radius, 0.f, 90.f, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);
    build_arc_filled_mesh(BR, radius, radius, 270.f, 360.f, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);

    Immediate2DRenderer::build_rectangle_filled_mesh(
        gs_vec2f(_Min.x + 0.f, _Max.y - radius),
        gs_vec2f(_Max.x - 0.f, _Min.y + radius),
        _Color,
        m_RenderingQueue->get_default_texture(),
        m_Vertexes,
        m_Indexes);

    Immediate2DRenderer::build_rectangle_filled_mesh(
        gs_vec2f(_Min.x + radius, _Max.y - 0.f),
        gs_vec2f(_Max.x - radius, _Min.y + 0.f),
        _Color,
        m_RenderingQueue->get_default_texture(),
        m_Vertexes,
        m_Indexes);

    push_rendering_command(m_RenderingQueue->get_default_texture(), _Color, _Transform);
}

void Immediate2DRenderer::push_text(
    const std::u32string&     _Text,
    const float&              _Size,
    const gs_vec4f&           _Color,
    const float&              _Depth,
    const gs_vec2f&           _Position,
    const float&              _Rotation,
    const gs_vec2f&           _Scale,
    const RenderingQueueFont& _Font)
{
    RenderingQueueFont font = _Font.is_null() ? m_RenderingQueue->get_default_font() : _Font;

    float scale     = _Size / (float)font.SizeInPixels;
    float offset    = (font.Ascent + font.Descent + font.LineGap) * scale;
    float positionX = 0.f;
    float positionY = -offset;

    for(int i = 0; i < (int)_Text.size(); ++i)
    {
        unsigned int symbol = _Text[i];

        // fallbacks
        if(!font.contains_glyph(symbol))
        {
            // next line
            if(symbol == '\n')
            {
                positionY -= gs_max(_Size, gs_abs(offset));
                positionX =  0.f;
            }
            // carriage return
            else if(symbol == '\r')
                positionX =  0.f;
            // tab
            else if(symbol == '\t')
                positionX += _Size;
            else
            {
                // TODO: do someting here...
                // May be use fallback font and take fallback character from there ???
            }

            continue;
        }

        RenderingQueueGlyph glyph    = font.retrieve_glyph(symbol);
        float glyphWidth             = glyph.Box.get_size().x * scale;
        float glyphHeight            = glyph.Box.get_size().y * scale;
        float glyphHorizontalBearing = glyph.Bearing.x * scale;
        float glyphVerticalBearing   = glyph.Bearing.y * scale;
        float glyphAdvance           = glyph.Advance * scale;

        build_rectangle_filled_mesh(
            gs_vec2f(positionX + glyphHorizontalBearing, positionY - glyphVerticalBearing),
            gs_vec2f(positionX + glyphHorizontalBearing + glyphWidth, positionY - glyphVerticalBearing - glyphHeight),
            glyph.MinUV,
            glyph.MaxUV,
            _Color,
            m_Vertexes,
            m_Indexes
        );

        // move cursor
        positionX += glyphAdvance;
    }

    push_rendering_command(
        font.AtlasTexture,
        _Color,
        calculate_transform_matrix(_Depth, _Position, _Rotation, _Scale));
}

void Immediate2DRenderer::push_text(
    const std::u16string&     _Text,
    const float&              _Size,
    const gs_vec4f&           _Color,
    const float&              _Depth,
    const gs_vec2f&           _Position,
    const float&              _Rotation,
    const gs_vec2f&           _Scale,
    const RenderingQueueFont& _Font)
{
    push_text(
        Frenchie::Core::String::convert_utf16_to_utf8(_Text),
        _Size,
        _Color,
        _Depth,
        _Position,
        _Rotation,
        _Scale,
        _Font
    );
}

void Immediate2DRenderer::push_text(
    const std::string&        _Text,
    const float&              _Size,
    const gs_vec4f&           _Color,
    const float&              _Depth,
    const gs_vec2f&           _Position,
    const float&              _Rotation,
    const gs_vec2f&           _Scale,
    const RenderingQueueFont& _Font)
{
    push_text(
        Frenchie::Core::String::convert_utf8_to_utf32(_Text),
        _Size,
        _Color,
        _Depth,
        _Position,
        _Rotation,
        _Scale,
        _Font
    );
}

void Immediate2DRenderer::push_arc_filled(
    const gs_vec2f&              _Center,
    const float&                 _MinorRadius,
    const float&                 _MajorRadius,
    const float&                 _SourceAngle,
    const float&                 _TargetAngle,
    const gs_vec4f&              _Color,
    const float&                 _Depth,
    const gs_vec2f&              _Position,
    const float&                 _Rotation,
    const gs_vec2f&              _Scale,
    const RenderingQueueTexture& _Texture)
{
    gs_mat4f _Transform =
        calculate_transform_matrix(_Depth, _Position, _Rotation, _Scale);

    // check that we are within viewport
    if(!m_Viewport.overlaps(
            gs_2dboxf(
                _Transform * gs_vec4f((_Center - gs_vec2f(_MinorRadius, _MajorRadius)), _Depth, 1.f),
                _Transform * gs_vec4f((_Center + gs_vec2f(_MinorRadius, _MajorRadius)), _Depth, 1.f))))
    {
        return;
    }

    Immediate2DRenderer::build_arc_filled_mesh(
        _Center,
        _MinorRadius,
        _MajorRadius,
        _SourceAngle,
        _TargetAngle,
        _Color,
        !_Texture.is_null() ? _Texture : m_RenderingQueue->get_default_texture(),
        m_Vertexes,
        m_Indexes);

    push_rendering_command(
        !_Texture.is_null() ? _Texture : m_RenderingQueue->get_default_texture(),
        _Color,
        _Transform);
}

void Immediate2DRenderer::push_line(
    const gs_vec2f& _P1,
    const gs_vec2f& _P2,
    const float&    _Width,
    const gs_vec4f& _Color,
    const float&    _Depth,
    const gs_vec2f& _Position,
    const float&    _Rotation,
    const gs_vec2f& _Scale)
{
    gs_mat4f _Transform =
        calculate_transform_matrix(_Depth, _Position, _Rotation, _Scale);

    if(!m_Viewport.contains(_Transform * gs_vec4f(_P1, _Depth, 1.f)) &&
        !m_Viewport.contains(_Transform * gs_vec4f(_P2, _Depth, 1.f)))
    {
        return;
    }

    Immediate2DRenderer::build_line_mesh(
        _P1,
        _P2,
        _Width,
        _Color,
        m_RenderingQueue->get_default_texture(),
        m_Vertexes,
        m_Indexes);

    push_rendering_command(m_RenderingQueue->get_default_texture(), _Color, _Transform);
}

void Immediate2DRenderer::push_arc(
    const gs_vec2f& _Center,
    const float&    _MinorRadius,
    const float&    _MajorRadius,
    const float&    _SourceAngle,
    const float&    _TargetAngle,
    const float&    _Width,
    const gs_vec4f& _Color,
    const float&    _Depth,
    const gs_vec2f& _Position,
    const float&    _Rotation,
    const gs_vec2f& _Scale)
{
    gs_mat4f _Transform =
        calculate_transform_matrix(_Depth, _Position, _Rotation, _Scale);

    // check that we are within viewport
    if(!m_Viewport.overlaps(
            gs_2dboxf(
                _Transform * gs_vec4f((_Center - gs_vec2f(_MinorRadius, _MajorRadius)), _Depth, 1.f),
                _Transform * gs_vec4f((_Center + gs_vec2f(_MinorRadius, _MajorRadius)), _Depth, 1.f))))
    {
        return;
    }

    Immediate2DRenderer::build_arc_mesh(
        _Center,
        _MinorRadius,
        _MajorRadius,
        _SourceAngle,
        _TargetAngle,
        _Width,
        _Color,
        m_RenderingQueue->get_default_texture(),
        m_Vertexes,
        m_Indexes);

    push_rendering_command(m_RenderingQueue->get_default_texture(), _Color, _Transform);
}

void Immediate2DRenderer::push_triangle(
    const gs_vec2f& _P1,
    const gs_vec2f& _P2,
    const gs_vec2f& _P3,
    const float&    _Width,
    const gs_vec4f& _Color,
    const float&    _Depth,
    const gs_vec2f& _Position,
    const float&    _Rotation,
    const gs_vec2f& _Scale)
{
    gs_mat4f _Transform =
        calculate_transform_matrix(_Depth, _Position, _Rotation, _Scale);

    // check if we are within viewport
    if(!m_Viewport.contains(_Transform * gs_vec4f(_P1, _Depth, 1.f)) &&
        !m_Viewport.contains(_Transform * gs_vec4f(_P2, _Depth, 1.f)) &&
        !m_Viewport.contains(_Transform * gs_vec4f(_P3, _Depth, 1.f)))
    {
        return;
    }

    // build mesh
    build_line_mesh(_P1, _P2, _Width, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);
    build_line_mesh(_P2, _P3, _Width, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);
    build_line_mesh(_P3, _P1, _Width, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);

    // push rendering command
    push_rendering_command(m_RenderingQueue->get_default_texture(), _Color, _Transform);
}

void Immediate2DRenderer::push_rectangle(
    const gs_vec2f& _Min,
    const gs_vec2f& _Max,
    const float&    _Width,
    const gs_vec4f& _Color,
    const float&    _Depth,
    const gs_vec2f& _Position,
    const float&    _Rotation ,
    const gs_vec2f& _Scale)
{
    gs_mat4f _Transform =
        calculate_transform_matrix(_Depth, _Position, _Rotation, _Scale);

    // check that we are within viewport
    if(!m_Viewport.overlaps(
        gs_2dboxf(
            _Transform * gs_vec4f(_Min, _Depth, 1.f),
            _Transform * gs_vec4f(_Max, _Depth, 1.f))))
    {
        return;
    }

    // build mesh
    const gs_vec2f _P1 = gs_vec2f(_Min.x, _Min.y);
    const gs_vec2f _P2 = gs_vec2f(_Max.x, _Min.y);
    const gs_vec2f _P3 = gs_vec2f(_Max.x, _Max.y);
    const gs_vec2f _P4 = gs_vec2f(_Min.x, _Max.y);
    build_line_mesh(_P1, _P2, _Width, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);
    build_line_mesh(_P1, _P2, _Width, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);
    build_line_mesh(_P2, _P3, _Width, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);
    build_line_mesh(_P3, _P4, _Width, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);
    build_line_mesh(_P4, _P1, _Width, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);

    // push rendering command
    push_rendering_command(m_RenderingQueue->get_default_texture(), _Color, _Transform);
}

void Immediate2DRenderer::push_rectangle_rounded(
    const gs_vec2f& _Min,
    const gs_vec2f& _Max,
    const float&    _Radius,
    const float&    _Width,
    const gs_vec4f& _Color,
    const float&    _Depth,
    const gs_vec2f& _Position,
    const float&    _Rotation,
    const gs_vec2f& _Scale)
{
    gs_mat4f _Transform =
        calculate_transform_matrix(_Depth, _Position, _Rotation, _Scale);

    // check that we are within viewport
    if(!m_Viewport.overlaps(
        gs_2dboxf(
            _Transform * gs_vec4f(_Min, _Depth, 1.f, 1.f),
            _Transform * gs_vec4f(_Max, _Depth, 1.f, 1.f))))
    {
        return;
    }

    // check rounding radius
    if(_Radius <= gs_max(_Width, 4.f))
    {
        return push_rectangle(
            _Min,
            _Max,
            _Width,
            _Color,
            _Depth,
            _Position,
            _Rotation,
            _Scale);
    }

    // compute radius
    float radius = gs_min(gs_min(_Radius, gs_abs(_Max.x - _Min.x) * 0.5f), gs_min(_Radius, gs_abs(_Max.y - _Min.y) * 0.5f));

    // points
    gs_vec2f TL = gs_vec2f(_Min.x + radius, _Max.y - radius);
    gs_vec2f BL = gs_vec2f(_Min.x + radius, _Min.y + radius);
    gs_vec2f TR = gs_vec2f(_Max.x - radius, _Max.y - radius);
    gs_vec2f BR = gs_vec2f(_Max.x - radius, _Min.y + radius);

    // sides
    build_arc_mesh(TL, radius, radius, 90.f, 180.f, _Width, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);
    build_arc_mesh(BL, radius, radius, 180.f, 270.f, _Width, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);
    build_arc_mesh(TR, radius, radius, 0.f, 90.f, _Width, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);
    build_arc_mesh(BR, radius, radius, 270.f, 360.f, _Width, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);
    build_line_mesh(calculate_arc_point(TL, radius, radius, 180), calculate_arc_point(BL, radius, radius, 180), _Width, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);
    build_line_mesh(calculate_arc_point(TL, radius, radius, 90), calculate_arc_point(TR, radius, radius, 90), _Width, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);
    build_line_mesh(calculate_arc_point(TR, radius, radius, 0), calculate_arc_point(BR, radius, radius, 0), _Width, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);
    build_line_mesh(calculate_arc_point(BL, radius, radius, 270), calculate_arc_point(BR, radius, radius, 270), _Width, _Color, m_RenderingQueue->get_default_texture(), m_Vertexes, m_Indexes);

    push_rendering_command(m_RenderingQueue->get_default_texture(), _Color, _Transform);
}

void Immediate2DRenderer::build_triangle_filled_mesh(
    const gs_vec2f&                    _P1,
    const gs_vec2f&                    _P2,
    const gs_vec2f&                    _P3,
    const gs_vec4f&                    _Color,
    const RenderingQueueTexture&       _Texture,
    std::vector<RenderingQueueVertex>& _Vertexes,
    std::vector<int>&                  _Indexes)
{
    const int size = (int)_Vertexes.size();

    _Vertexes.push_back(
        RenderingQueueVertex(
            gs_vec3f(_P1.x, _P1.y, 0.f),
            gs_vec3f(0.f), gs_vec2f(_P1.x / _Texture.Width, _P1.y / _Texture.Height),
            _Color));
    
    _Vertexes.push_back(
        RenderingQueueVertex(
            gs_vec3f(_P2.x, _P2.y, 0.f),
            gs_vec3f(0.f), gs_vec2f(_P2.x / _Texture.Width, _P2.y / _Texture.Height),
            _Color));
    
    _Vertexes.push_back(
        RenderingQueueVertex(
            gs_vec3f(_P3.x, _P3.y, 0.f),
            gs_vec3f(0.f),
            gs_vec2f(_P3.x / _Texture.Width, _P3.y / _Texture.Height),
            _Color));
    
    for (int i = size; i < (int)_Vertexes.size(); ++i)
        _Indexes.push_back(i);
}

void Immediate2DRenderer::build_rectangle_filled_mesh(
    const gs_vec2f&                    _Min,
    const gs_vec2f&                    _Max,
    const gs_vec4f&                    _Color,
    const RenderingQueueTexture&       _Texture,
    std::vector<RenderingQueueVertex>& _Vertexes,
    std::vector<int>&                  _Indexes)
{
    const int      size   = (int)_Vertexes.size();
    const gs_vec3f point1 = gs_vec3f(_Min.x, _Min.y, 0.f);
    const gs_vec3f point2 = gs_vec3f(_Max.x, _Min.y, 0.f);
    const gs_vec3f point3 = gs_vec3f(_Max.x, _Max.y, 0.f);
    const gs_vec3f point4 = gs_vec3f(_Min.x, _Max.y, 0.f);

    // triangle 1
    _Vertexes.push_back(
        RenderingQueueVertex(
            point1,
            gs_vec3f(0.f),
            gs_vec2f(point1.x / _Texture.Width, point1.y / _Texture.Height),
            _Color));

    _Vertexes.push_back(
        RenderingQueueVertex(
            point2,
            gs_vec3f(0.f),
            gs_vec2f(point2.x / _Texture.Width, point2.y / _Texture.Height),
            _Color));

    _Vertexes.push_back(
        RenderingQueueVertex(
            point4,
            gs_vec3f(0.f),
            gs_vec2f(point4.x / _Texture.Width, point4.y / _Texture.Height),
            _Color));

    // triangle 2
    _Vertexes.push_back(
        RenderingQueueVertex(
            point2,
            gs_vec3f(0.f),
            gs_vec2f(point2.x / _Texture.Width, point2.y / _Texture.Height),
            _Color));

    _Vertexes.push_back(
        RenderingQueueVertex(
            point3, gs_vec3f(0.f),
            gs_vec2f(point3.x / _Texture.Width, point3.y / _Texture.Height),
            _Color));

    _Vertexes.push_back(
        RenderingQueueVertex(
            point4,
            gs_vec3f(0.f),
            gs_vec2f(point4.x / _Texture.Width, point4.y / _Texture.Height),
            _Color));

    for (int i = size; i < (int)_Vertexes.size(); ++i)
        _Indexes.push_back(i);
}

void Immediate2DRenderer::build_rectangle_filled_mesh(
    const gs_vec2f&                    _Min,
    const gs_vec2f&                    _Max,
    const gs_vec2f&                    _MinUV,
    const gs_vec2f&                    _MaxUV,
    const gs_vec4f&                    _Color,
    std::vector<RenderingQueueVertex>& _Vertexes,
    std::vector<int>&                  _Indexes)
{
    const int size = (int)_Vertexes.size();

    const gs_vec3f _P1 = gs_vec3f(_Min.x, _Min.y, 0.f);
    const gs_vec3f _P2 = gs_vec3f(_Max.x, _Min.y, 0.f);
    const gs_vec3f _P3 = gs_vec3f(_Max.x, _Max.y, 0.f);
    const gs_vec3f _P4 = gs_vec3f(_Min.x, _Max.y, 0.f);

    const gs_vec3f _UV1 = gs_vec3f(_MinUV.x, _MinUV.y, 0.f);
    const gs_vec3f _UV2 = gs_vec3f(_MaxUV.x, _MinUV.y, 0.f);
    const gs_vec3f _UV3 = gs_vec3f(_MaxUV.x, _MaxUV.y, 0.f);
    const gs_vec3f _UV4 = gs_vec3f(_MinUV.x, _MaxUV.y, 0.f);

    // triangle 1
    _Vertexes.push_back(
        RenderingQueueVertex(
            _P1,
            gs_vec3f(0.f),
            gs_vec2f(_UV1.x, _UV1.y),
            _Color));

    _Vertexes.push_back(
        RenderingQueueVertex(
            _P2,
            gs_vec3f(0.f),
            gs_vec2f(_UV2.x, _UV2.y),
            _Color));

    _Vertexes.push_back(
        RenderingQueueVertex(
            _P4,
            gs_vec3f(0.f),
            gs_vec2f(_UV4.x, _UV4.y),
            _Color));

    // triangle 2
    _Vertexes.push_back(
        RenderingQueueVertex(
            _P2,
            gs_vec3f(0.f),
            gs_vec2f(_UV2.x, _UV2.y),
            _Color));

    _Vertexes.push_back(
        RenderingQueueVertex(
            _P3,
            gs_vec3f(0.f),
            gs_vec2f(_UV3.x, _UV3.y),
            _Color));

    _Vertexes.push_back(
        RenderingQueueVertex(
            _P4,
            gs_vec3f(0.f),
            gs_vec2f(_UV4.x, _UV4.y),
            _Color));

    for (int i = size; i < (int)_Vertexes.size(); ++i)
        _Indexes.push_back(i);
}

void Immediate2DRenderer::build_arc_filled_mesh(
    const gs_vec2f&                    _Center,
    const float&                       _MinorRadius,
    const float&                       _MajorRadius,
    const float&                       _SourceAngle,
    const float&                       _TargetAngle,
    const gs_vec4f&                    _Color,
    const RenderingQueueTexture&       _Texture,
    std::vector<RenderingQueueVertex>& _Vertexes,
    std::vector<int>&                  _Indexes,
    const int&                         _SegmentsCount)
{
    gs_vec2f p0 = gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(_SourceAngle)), _Center.y + _MajorRadius * sin(gs_to_radians(_SourceAngle)));
    gs_vec2f p1 = p0;
    gs_vec2f p2 = p0;

    const float angleIncrement = _TargetAngle / 36.f;

    for (float angle = _SourceAngle; angle <= _TargetAngle; angle += angleIncrement, p1 = p2)
    {
        p2 = gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(angle)), _Center.y + _MajorRadius * sin(gs_to_radians(angle)));

        Immediate2DRenderer::build_triangle_filled_mesh(
            _Center,
            p1,
            p2,
            _Color,
            _Texture,
            _Vertexes,
            _Indexes);
    }


    // gs_vec2f p0 = gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(_SourceAngle)), _Center.y + _MajorRadius * sin(gs_to_radians(_SourceAngle)));
    // gs_vec2f p1 = p0;
    // gs_vec2f p2 = p0;

    // gs_complex<float> rotate =
    // {
    //     cos(gs_to_radians(_TargetAngle / _SegmentsCount)),
    //     sin(gs_to_radians(_TargetAngle / _SegmentsCount))
    // };

    // gs_complex<float> angle = 
    // {
    //     cos(gs_to_radians(_SourceAngle)),
    //     sin(gs_to_radians(_SourceAngle))
    // };

    // for (int i = 0; i < _SegmentsCount; angle *= rotate, p1 = p2, ++i)
    // {
    //     p2 = gs_vec2f(_Center.x + _MinorRadius * gs_realf(angle), _Center.y + _MajorRadius * gs_imagf(angle));
        
    //     Immediate2DRenderer::build_triangle_filled_mesh(
    //         _Center,
    //         p1,
    //         p2,
    //         _Color,
    //         _Texture,
    //         _Vertexes,
    //         _Indexes);
    // }
}

void Immediate2DRenderer::build_line_mesh(
    const gs_vec2f&                    _P1,
    const gs_vec2f&                    _P2,
    const float&                       _Width,
    const gs_vec4f&                    _Color,
    const RenderingQueueTexture&       _Texture,
    std::vector<RenderingQueueVertex>& _Vertexes,
    std::vector<int>&                  _Indexes)
{
    const gs_vec3f p1 = gs_vec3f(_P1.x, _P1.y, 0.f);
    const gs_vec3f p2 = gs_vec3f(_P2.x, _P2.y, 0.f);

    gs_vec3f direction     = gs_vector_normalize(_P2 - _P1);
    gs_vec2f perpendicular = gs_vector_normalize(gs_vector_cross(direction, gs_vec3f(0.f, 0.f, 1.f))) * _Width * 0.5f;

    if(_Width > 4.f)
    {
        Immediate2DRenderer::build_arc_filled_mesh(
            _P1,
            _Width * 0.5f,
            _Width * 0.5f,
            90.f,
            180.f,
            _Color,
            _Texture,
            _Vertexes,
            _Indexes
        );

        Immediate2DRenderer::build_arc_filled_mesh(
            _P2,
            _Width * 0.5f,
            _Width * 0.5f,
            270.f,
            360.f,
            _Color,
            _Texture,
            _Vertexes,
            _Indexes
        );
    }

    Immediate2DRenderer::build_triangle_filled_mesh(
        _P1 - perpendicular,
        _P2 - perpendicular,
        _P1 + perpendicular,
        _Color,
        _Texture,
        _Vertexes,
        _Indexes
    );

    Immediate2DRenderer::build_triangle_filled_mesh(
        _P1 + perpendicular,
        _P2 - perpendicular,
        _P2 + perpendicular,
        _Color,
        _Texture,
        _Vertexes,
        _Indexes
    );
}

void Immediate2DRenderer::build_arc_mesh(
    const gs_vec2f&                    _Center,
    const float&                       _MinorRadius,
    const float&                       _MajorRadius,
    const float&                       _SourceAngle,
    const float&                       _TargetAngle,
    const float&                       _LineWidth,
    const gs_vec4f&                    _Color,
    const RenderingQueueTexture&       _Texture,
    std::vector<RenderingQueueVertex>& _Vertexes,
    std::vector<int>&                  _Indexes)
{
    gs_vec2f p0 = gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(_SourceAngle)), _Center.y + _MajorRadius * sin(gs_to_radians(_SourceAngle)));
    gs_vec2f p1 = p0;
    gs_vec2f p2 = p0;

    const float angleIncrement = _TargetAngle / 36.f;

    for (float angle = _SourceAngle; angle <= _TargetAngle; angle += angleIncrement, p1 = p2)
    {
        p2 = gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(angle)), _Center.y + _MajorRadius * sin(gs_to_radians(angle)));
        build_line_mesh(p1, p2, _LineWidth, _Color, _Texture, _Vertexes, _Indexes);
    }
}