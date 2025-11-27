#include <FrenchieApplicationImmediate2DRenderer.hpp>

using namespace Frenchie::Application;

Immediate2DRenderer::Immediate2DRenderer(){}
Immediate2DRenderer::~Immediate2DRenderer(){}

bool Immediate2DRenderer::awake()
{
    m_DefaultFont = Frenchie::Application::application_rendering_queue()->construct_font(
        "C:/SDK/Qt_Projects/OpenGL/shared/appData/fonts/Alice-Regular.ttf", 128
    );

    // register default shader here
    m_DefaultShader = Frenchie::Application::application_rendering_queue()->construct_shader(
        {
            // Vertex shader
            {
                std::string(
R"(
#version 330 core

// vertex attributes
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_UV;

// outputs
out vec3 Normal;
out vec2 UV;

// uniforms
uniform mat4 u_ModelMatrix;
uniform mat4 u_CameraViewMatrix;
uniform mat4 u_ProjectionMatrix;

void main()
{
// setup position
gl_Position = u_ProjectionMatrix * u_CameraViewMatrix * u_ModelMatrix * vec4(a_Position, 1.0);

// setup outputs
Normal = a_Normal;
UV     = a_UV;
}            
)"),
                RenderingQueueShaderType_::RenderingQueueShaderType_Vertex
            },

            // fragment shader
            {
                std::string(
R"(
#version 330 core

// inputs
in vec2 UV;

//outputs
out vec4 fragColor;

//uniforms
uniform vec4      u_Color;
uniform sampler2D u_Texture;

void main()
{
    fragColor = u_Color * texture(u_Texture, UV);
}
)"),
                RenderingQueueShaderType_::RenderingQueueShaderType_Fragment
                },
        }
    );    

    // create default white pattern texture
    const int     height   = 4;
    const int     width    = 4;
    const int     channels = 4;
    const int     red      = 0;
    const int     green    = 1;
    const int     blue     = 2;
    const int     alpha    = 3;
    unsigned char image[width * height * channels]{};

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            image[channels * (y * width + x) + red  ] = 255;
            image[channels * (y * width + x) + green] = 255;
            image[channels * (y * width + x) + blue ] = 255;
            image[channels * (y * width + x) + alpha] = 255;
        }
    }

    m_DefaultTexture = Frenchie::Application::application_rendering_queue()->construct_texture(image, width, height);

    return true;
}

void Immediate2DRenderer::frame_start()
{
    // update viewport bouinding rectangle
    {
        m_Viewport = gs_rectf(
            -Frenchie::Application::application()->get_window_size().x,
            -Frenchie::Application::application()->get_window_size().y,
            +Frenchie::Application::application()->get_window_size().x,
            +Frenchie::Application::application()->get_window_size().y
        );
    }

    // compute projection matrix
    {
        // compute projection matrix
        float width  = Frenchie::Application::application()->get_window_size().x;
        float height = Frenchie::Application::application()->get_window_size().y;
        float left   = -width  * 0.5f + width  * 0.5f; // The x-coordinate of the left edge of the viewable area.
        float right  = +width  * 0.5f + width  * 0.5f; // The x-coordinate of the right edge of the viewable area.
        float bottom = -height * 0.5f - height * 0.5f; // The y-coordinate of the bottom edge of the viewable area.
        float top    = +height * 0.5f - height * 0.5f; // The y-coordinate of the top edge of the viewable area.

        Frenchie::Application::application_rendering_queue()
            ->set_projection_matrix(gs_matrix_ortho(left, right, bottom, top, -1000.0f, 1000.0f));

        // compute camera view matrix
        // TODO: implement logic
    }
}

void Immediate2DRenderer::frame_render()
{
    m_Vertexes.clear();
    m_Indexes.clear();
}

void Immediate2DRenderer::finish()
{
    Frenchie::Application::application_rendering_queue()->destroy_font(m_DefaultFont);
    Frenchie::Application::application_rendering_queue()->destroy_shader(m_DefaultShader);
    Frenchie::Application::application_rendering_queue()->destroy_texture(m_DefaultTexture);
}

void Immediate2DRenderer::push_rendering_command(const RenderingQueueTexture& _Texture, const gs_vec4f& _Color, const gs_mat4f& _Transform)
{
    if(m_Indexes.empty() || m_Vertexes.empty()) return;

    // push rendering command
    Frenchie::Application::application_rendering_queue()->push_command(
        // construct mesh
        Frenchie::Application::application_rendering_queue()->construct_mesh(
            &m_Vertexes[0],
            (int)m_Vertexes.size(),
            &m_Indexes[0],
            (int)m_Indexes.size()),
        
        // provide default shader
        m_DefaultShader,

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

gs_vec2f Immediate2DRenderer::calculate_text_size(
    const std::string&        _Text,
    const float&              _Size,
    const RenderingQueueFont& _Font)
{
    RenderingQueueFont font = _Font.is_null() ? m_DefaultFont : _Font;

    float scale     = _Size / (float)font.SizeInPixels;
    float offset    = gs_max((font.Ascent + font.LineGap) * scale, _Size);
    float positionX = 0;
    float positionY = -offset;
    float width     = gs_tiny<float>();
    float height    = gs_tiny<float>();

    for(int i = 0; i < strlen(_Text.c_str()); ++i)
    {
        height = gs_max(height, gs_abs(positionY));
        width  = gs_max(width, positionX);

        // fallbacks
        if(!font.contains_glyph(_Text.c_str()[i]))
        {
            // next line
            if(_Text.c_str()[i] == '\n')
            {
                positionY -= gs_max(_Size, offset);
                positionX =  0.f;
            }
            // carriage return
            else if(_Text.c_str()[i] == '\r')
            {
                positionX = 0.f;
            }
            // tab
            else if(_Text.c_str()[i] == '\t')
            {
                positionX += _Size;
            }
            else // fallback
            {
                // TODO: do someting here...
                // May be use fallback font and take fallback character from there ???
            }

            continue;
        }

        RenderingQueueGlyph glyph = font.retrieve_glyph(_Text.c_str()[i]);
        positionX += glyph.Advance * scale;
    }

    height = gs_max(height, gs_abs(positionY));
    width  = gs_max(width, positionX);

    return {width, height};
}

void Immediate2DRenderer::push_triangle_filled(
    const gs_vec2f&              _P1,
    const gs_vec2f&              _P2,
    const gs_vec2f&              _P3,
    const float&                 _Depth,
    const gs_vec4f&              _Color,
    const gs_mat4f&              _Transform,
    const RenderingQueueTexture& _Texture)
{
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
        _Depth,
        !_Texture.is_null() ? _Texture : m_DefaultTexture,
        m_Vertexes,
        m_Indexes);

    push_rendering_command(
        !_Texture.is_null() ? _Texture : m_DefaultTexture,
        _Color,
        _Transform
    );
}

void Immediate2DRenderer::push_rectangle_filled(
    const gs_vec2f&              _Min,
    const gs_vec2f&              _Max,
    const float&                 _Depth,
    const gs_vec4f&              _Color,
    const gs_mat4f&              _Transform,
    const RenderingQueueTexture& _Texture)
{
    if(!m_Viewport.overlaps(
        gs_rectf(
            _Transform * gs_vec4f(_Min, _Depth, 1.f),
            _Transform * gs_vec4f(_Max, _Depth, 1.f))))
    {
        return;
    }

    Immediate2DRenderer::build_rectangle_filled_mesh(
        _Min,
        _Max,
        _Depth,
        !_Texture.is_null() ? _Texture : m_DefaultTexture,
        m_Vertexes,
        m_Indexes);

    push_rendering_command(
        !_Texture.is_null() ? _Texture : m_DefaultTexture,
        _Color,
        _Transform);
}

void Immediate2DRenderer::push_text(
    const std::string&        _Text,
    const float&              _Size,
    const gs_vec2f&           _Position,
    const gs_vec4f&           _Color,
    const gs_mat4f&           _Transform,
    const RenderingQueueFont& _Font)
{
    RenderingQueueFont font = _Font.is_null() ? m_DefaultFont : _Font;

    float scale     = _Size / (float)font.SizeInPixels;
    float offset    = (font.Ascent + font.LineGap) * scale;
    float positionX = _Position.x;
    float positionY = _Position.y - offset;

    for(int i = 0; i < strlen(_Text.c_str()); ++i)
    {
        // fallbacks
        if(!font.contains_glyph(_Text.c_str()[i]))
        {
            // next line
            if(_Text.c_str()[i] == '\n')
            {
                positionY -= gs_max(_Size, offset);
                positionX =  _Position.x;
            }
            // carriage return
            else if(_Text.c_str()[i] == '\r')
                positionX =  _Position.x;
            // tab
            else if(_Text.c_str()[i] == '\t')
                positionX += _Size;
            else
            {
                // TODO: do someting here...
                // May be use fallback font and take fallback character from there ???
            }

            continue;
        }

        RenderingQueueGlyph glyph    = font.retrieve_glyph(_Text.c_str()[i]);
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
            0.f,
            m_Vertexes,
            m_Indexes
        );

        // move cursor
        positionX += glyphAdvance;
    }

    push_rendering_command(font.AtlasTexture, _Color, _Transform);
}

void Immediate2DRenderer::push_line(
    const gs_vec2f& _P1,
    const gs_vec2f& _P2,
    const float&    _Depth,
    const float&    _LineWidth,
    const gs_vec4f& _Color,
    const gs_mat4f& _Transform)
{
    if(!m_Viewport.contains(_Transform * gs_vec4f(_P1, _Depth, 1.f)) &&
        !m_Viewport.contains(_Transform * gs_vec4f(_P2, _Depth, 1.f)))
    {
        return;
    }

    Immediate2DRenderer::build_line_mesh(
        _P1,
        _P2,
        _Depth,
        _LineWidth,
        m_DefaultTexture,
        m_Vertexes,
        m_Indexes);

    push_rendering_command(m_DefaultTexture, _Color, _Transform);
}

void Immediate2DRenderer::push_arc(
    const gs_vec2f& _Center,
    const float&    _MinorRadius,
    const float&    _MajorRadius,
    const float&    _SourceAngle,
    const float&    _TargetAngle,
    const float&    _Depth,
    const float&    _LineWidth,
    const gs_vec4f& _Color,
    const gs_mat4f& _Transform)
{
    // check that we are within viewport
    if(!m_Viewport.overlaps(
            gs_rectf(
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
        _Depth,
        _LineWidth,
        m_DefaultTexture,
        m_Vertexes,
        m_Indexes);

    push_rendering_command(m_DefaultTexture, _Color, _Transform);
}

void Immediate2DRenderer::push_triangle(
    const gs_vec2f& _P1,
    const gs_vec2f& _P2,
    const gs_vec2f& _P3,
    const float&    _Depth,
    const float&    _LineWidth,
    const gs_vec4f& _Color,
    const gs_mat4f& _Transform)
{
    // check if we are within viewport
    if(!m_Viewport.contains(_Transform * gs_vec4f(_P1, _Depth, 1.f)) &&
        !m_Viewport.contains(_Transform * gs_vec4f(_P2, _Depth, 1.f)) &&
        !m_Viewport.contains(_Transform * gs_vec4f(_P3, _Depth, 1.f)))
    {
        return;
    }

    // build mesh
    build_line_mesh(_P1, _P2, _Depth, _LineWidth, m_DefaultTexture, m_Vertexes, m_Indexes);
    build_line_mesh(_P2, _P3, _Depth, _LineWidth, m_DefaultTexture, m_Vertexes, m_Indexes);
    build_line_mesh(_P3, _P1, _Depth, _LineWidth, m_DefaultTexture, m_Vertexes, m_Indexes);

    // push rendering command
    push_rendering_command(m_DefaultTexture, _Color, _Transform);
}

void Immediate2DRenderer::push_rectangle(
    const gs_vec2f& _Min,
    const gs_vec2f& _Max,
    const float&    _Depth,
    const float&    _LineWidth,
    const gs_vec4f& _Color,
    const gs_mat4f& _Transform)
{
    // check that we are within viewport
    if(!m_Viewport.overlaps(
        gs_rectf(
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
    build_line_mesh(_P1, _P2, _Depth, _LineWidth, m_DefaultTexture, m_Vertexes, m_Indexes);
    build_line_mesh(_P1, _P2, _Depth, _LineWidth, m_DefaultTexture, m_Vertexes, m_Indexes);
    build_line_mesh(_P2, _P3, _Depth, _LineWidth, m_DefaultTexture, m_Vertexes, m_Indexes);
    build_line_mesh(_P3, _P4, _Depth, _LineWidth, m_DefaultTexture, m_Vertexes, m_Indexes);
    build_line_mesh(_P4, _P1, _Depth, _LineWidth, m_DefaultTexture, m_Vertexes, m_Indexes);

    // push rendering command
    push_rendering_command(m_DefaultTexture, _Color, _Transform);
}

void Immediate2DRenderer::push_rectangle_rounded(
    const gs_vec2f& _Min,
    const gs_vec2f& _Max,
    const float&    _Radius,
    const float&    _Depth,
    const float&    _LineWidth,
    const gs_vec4f& _Color,
    const gs_mat4f& _Transform)
{
    // check that we are within viewport
    if(!m_Viewport.overlaps(
        gs_rectf(
            _Transform * gs_vec4f(_Min, _Depth, 1.f),
            _Transform * gs_vec4f(_Max, _Depth, 1.f))))
    {
        return;
    }

    // check rounding radius
    if(_Radius <= gs_max(_LineWidth, 4.f))
    {
        push_rectangle(
            _Min,
            _Max,
            _Depth,
            _LineWidth,
            _Color,
            _Transform);

        return;
    }

    // compute radius
    float radius = gs_min(gs_min(_Radius, gs_abs(_Max.x - _Min.x) * 0.5f), gs_min(_Radius, gs_abs(_Max.y - _Min.y) * 0.5f));

    // auxiliary lambdas
    auto arc_point = [](
        const gs_vec2f& _Center,
        const float&    _MinorRadius,
        const float&    _MajorRadius,
        const float&    _ArcAngle
    )->gs_vec2f
    {
        return gs_vec2f(
            _Center.x + _MinorRadius * cos(gs_to_radians(_ArcAngle)),
            _Center.y + _MajorRadius * sin(gs_to_radians(_ArcAngle)));
    };

    // points
    gs_vec2f TL = gs_vec2f(_Min.x + radius, _Max.y - radius);
    gs_vec2f BL = gs_vec2f(_Min.x + radius, _Min.y + radius);
    gs_vec2f TR = gs_vec2f(_Max.x - radius, _Max.y - radius);
    gs_vec2f BR = gs_vec2f(_Max.x - radius, _Min.y + radius);

    // sides
    build_arc_mesh(TL, radius, radius, 90.f, 180.f, _Depth, _LineWidth, m_DefaultTexture, m_Vertexes, m_Indexes);
    build_arc_mesh(BL, radius, radius, 180.f, 270.f, _Depth, _LineWidth, m_DefaultTexture, m_Vertexes, m_Indexes);
    build_arc_mesh(TR, radius, radius, 0.f, 90.f, _Depth, _LineWidth, m_DefaultTexture, m_Vertexes, m_Indexes);
    build_arc_mesh(BR, radius, radius, 270.f, 360.f, _Depth, _LineWidth, m_DefaultTexture, m_Vertexes, m_Indexes);
    build_line_mesh(arc_point(TL, radius, radius, 180), arc_point(BL, radius, radius, 180), _Depth, _LineWidth, m_DefaultTexture, m_Vertexes, m_Indexes);
    build_line_mesh(arc_point(TL, radius, radius, 90), arc_point(TR, radius, radius, 90), _Depth, _LineWidth, m_DefaultTexture, m_Vertexes, m_Indexes);
    build_line_mesh(arc_point(TR, radius, radius, 0), arc_point(BR, radius, radius, 0), _Depth, _LineWidth, m_DefaultTexture, m_Vertexes, m_Indexes);
    build_line_mesh(arc_point(BL, radius, radius, 270), arc_point(BR, radius, radius, 270), _Depth, _LineWidth, m_DefaultTexture, m_Vertexes, m_Indexes);

    push_rendering_command(m_DefaultTexture, _Color, _Transform);
}

void Immediate2DRenderer::build_triangle_filled_mesh(
    const gs_vec2f&                    _P1,
    const gs_vec2f&                    _P2,
    const gs_vec2f&                    _P3,
    const float&                       _Depth,
    const RenderingQueueTexture&       _Texture,
    std::vector<RenderingQueueVertex>& _Vertexes,
    std::vector<int>&                  _Indexes)
{
    _Vertexes.push_back(
        RenderingQueueVertex(
            gs_vec3f(_P1.x, _P1.y, _Depth),
            gs_vec3f(0.f), gs_vec2f(_P1.x / _Texture.Width, _P1.y / _Texture.Height)));
    
    _Vertexes.push_back(
        RenderingQueueVertex(
            gs_vec3f(_P2.x, _P2.y, _Depth),
            gs_vec3f(0.f), gs_vec2f(_P2.x / _Texture.Width, _P2.y / _Texture.Height)));
    
    _Vertexes.push_back(
        RenderingQueueVertex(
            gs_vec3f(_P3.x, _P3.y, _Depth),
            gs_vec3f(0.f),
            gs_vec2f(_P3.x / _Texture.Width, _P3.y / _Texture.Height)));
    
    for (int i = 0; i < (int)_Vertexes.size(); ++i)
        _Indexes.push_back(i);
}

void Immediate2DRenderer::build_rectangle_filled_mesh(
    const gs_vec2f&                    _Min,
    const gs_vec2f&                    _Max,
    const float&                       _Depth,
    const RenderingQueueTexture&       _Texture,
    std::vector<RenderingQueueVertex>& _Vertexes,
    std::vector<int>&                  _Indexes)
{
    const gs_vec3f _P1 = gs_vec3f(_Min.x, _Min.y, _Depth);
    const gs_vec3f _P2 = gs_vec3f(_Max.x, _Min.y, _Depth);
    const gs_vec3f _P3 = gs_vec3f(_Max.x, _Max.y, _Depth);
    const gs_vec3f _P4 = gs_vec3f(_Min.x, _Max.y, _Depth);

    // triangle 1
    _Vertexes.push_back(
        RenderingQueueVertex(
            _P1,
            gs_vec3f(0.f),
            gs_vec2f(_P1.x / _Texture.Width, _P1.y / _Texture.Height)));

    _Vertexes.push_back(
        RenderingQueueVertex(
            _P2,
            gs_vec3f(0.f),
            gs_vec2f(_P2.x / _Texture.Width, _P2.y / _Texture.Height)));

    _Vertexes.push_back(
        RenderingQueueVertex(
            _P4,
            gs_vec3f(0.f),
            gs_vec2f(_P4.x / _Texture.Width, _P4.y / _Texture.Height)));

    // triangle 2
    _Vertexes.push_back(
        RenderingQueueVertex(
            _P2,
            gs_vec3f(0.f),
            gs_vec2f(_P2.x / _Texture.Width, _P2.y / _Texture.Height)));

    _Vertexes.push_back(
        RenderingQueueVertex(
            _P3, gs_vec3f(0.f),
            gs_vec2f(_P3.x / _Texture.Width, _P3.y / _Texture.Height)));

    _Vertexes.push_back(
        RenderingQueueVertex(
            _P4,
            gs_vec3f(0.f),
            gs_vec2f(_P4.x / _Texture.Width, _P4.y / _Texture.Height)));

    for (int i = 0; i < (int)_Vertexes.size(); ++i)
        _Indexes.push_back(i);
}

void Immediate2DRenderer::build_rectangle_filled_mesh(
    const gs_vec2f&                    _Min,
    const gs_vec2f&                    _Max,
    const gs_vec2f&                    _MinUV,
    const gs_vec2f&                    _MaxUV,
    const float&                       _Depth,
    std::vector<RenderingQueueVertex>& _Vertexes,
    std::vector<int>&                  _Indexes)
{
    const gs_vec3f _P1 = gs_vec3f(_Min.x, _Min.y, _Depth);
    const gs_vec3f _P2 = gs_vec3f(_Max.x, _Min.y, _Depth);
    const gs_vec3f _P3 = gs_vec3f(_Max.x, _Max.y, _Depth);
    const gs_vec3f _P4 = gs_vec3f(_Min.x, _Max.y, _Depth);

    const gs_vec3f _UV1 = gs_vec3f(_MinUV.x, _MinUV.y, _Depth);
    const gs_vec3f _UV2 = gs_vec3f(_MaxUV.x, _MinUV.y, _Depth);
    const gs_vec3f _UV3 = gs_vec3f(_MaxUV.x, _MaxUV.y, _Depth);
    const gs_vec3f _UV4 = gs_vec3f(_MinUV.x, _MaxUV.y, _Depth);

    // triangle 1
    _Vertexes.push_back(
        RenderingQueueVertex(
            _P1,
            gs_vec3f(0.f),
            gs_vec2f(_UV1.x, _UV1.y)));

    _Vertexes.push_back(
        RenderingQueueVertex(
            _P2,
            gs_vec3f(0.f),
            gs_vec2f(_UV2.x, _UV2.y)));

    _Vertexes.push_back(
        RenderingQueueVertex(
            _P4,
            gs_vec3f(0.f),
            gs_vec2f(_UV4.x, _UV4.y)));

    // triangle 2
    _Vertexes.push_back(
        RenderingQueueVertex(
            _P2,
            gs_vec3f(0.f),
            gs_vec2f(_UV2.x, _UV2.y)));

    _Vertexes.push_back(
        RenderingQueueVertex(
            _P3,
            gs_vec3f(0.f),
            gs_vec2f(_UV3.x, _UV3.y)));

    _Vertexes.push_back(
        RenderingQueueVertex(
            _P4,
            gs_vec3f(0.f),
            gs_vec2f(_UV4.x, _UV4.y)));

    for (int i = 0; i < (int)_Vertexes.size(); ++i)
        _Indexes.push_back(i);
}

void Immediate2DRenderer::build_line_mesh(
    const gs_vec2f&                    _P1,
    const gs_vec2f&                    _P2,
    const float&                       _Depth,
    const float&                       _LineWidth,
    const RenderingQueueTexture&       _Texture,
    std::vector<RenderingQueueVertex>& _Vertexes,
    std::vector<int>&                  _Indexes)
{
    const gs_vec3f p1 = gs_vec3f(_P1.x, _P1.y, _Depth);
    const gs_vec3f p2 = gs_vec3f(_P2.x, _P2.y, _Depth);

    gs_vec3f direction     = gs_vector_normalize(_P2 - _P1);
    gs_vec2f perpendicular = gs_vector_normalize(gs_vector_cross(direction, gs_vec3f(0.f, 0.f, 1.f))) * _LineWidth * 0.5f;

    build_triangle_filled_mesh(
        _P1 - perpendicular,
        _P2 - perpendicular,
        _P1 + perpendicular,
        _Depth,
        _Texture,
        _Vertexes,
        _Indexes
    );

    build_triangle_filled_mesh(
        _P1 + perpendicular,
        _P2 - perpendicular,
        _P2 + perpendicular,
        _Depth,
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
    const float&                       _Depth,
    const float&                       _LineWidth,
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
        build_line_mesh(p1, p2, _LineWidth, _LineWidth, _Texture, _Vertexes, _Indexes);
    }
}