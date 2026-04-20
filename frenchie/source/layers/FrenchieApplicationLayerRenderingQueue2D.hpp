#pragma once

#include <FrenchieApplicationLayerRenderingQueue.hpp>

namespace Frenchie
{
    namespace Application
    {
        struct RenderingQueue2DPathSegment
        {
        public:
            RenderingQueue2DPathSegment(const gs_vec2f& _P1, const gs_vec2f& _P2, const float& _Width = 4.f);
            void setup(const gs_vec2f& _P1, const gs_vec2f& _P2, const float& _Width);
            void setup(const float& _Width);

            gs_vec2f P1;
            gs_vec2f P1min;
            gs_vec2f P1max;

            gs_vec2f P2;
            gs_vec2f P2min;
            gs_vec2f P2max;

            int      Index{0};
        };

        struct RenderingQueue2DPathBuilder
        {
        public:

            RenderingQueue2DPathBuilder(const float& _PolygonLinesWidth);
            ~RenderingQueue2DPathBuilder();

            // TODO: add Bezier and random power curves here e.t.c
            void begin(const gs_vec2f& _Point);

            void line_to(const gs_vec2f& _Target);
            void arc_to(const gs_vec2f& _Target, const float& _Radius);

            void build_mesh(
                const gs_color&                                          _Color,
                const float&                                             _Width,
                std::vector<ApplicationRenderingBackendMeshVertex>&          _Vertexes,
                std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes,
                const ApplicationRenderingBackendTexture&                _Texture);

            void build_mesh_filled(
                const gs_color&                                          _Color,
                std::vector<ApplicationRenderingBackendMeshVertex>&          _Vertexes,
                std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes,
                const ApplicationRenderingBackendTexture&                _Texture);

        protected:

            std::vector<RenderingQueue2DPathSegment> m_PolygonLines           {std::vector<RenderingQueue2DPathSegment>()};
            float                                  m_PolygonLinesWidth      {4.f};
            std::vector<int>                       m_PolygonLinesIndexes    {std::vector<int>()};
            gs_vec2f                               m_PolygonLinesSourcePoint{gs_vec2f(0.f, 0.f)};

            // service methods
            void end();

            void build_triangle_filled_mesh(
                const gs_vec2f&                                          _P1,
                const gs_vec2f&                                          _P2,
                const gs_vec2f&                                          _P3,
                const gs_color&                                          _Color,
                const ApplicationRenderingBackendTexture&                _Texture,
                std::vector<ApplicationRenderingBackendMeshVertex>&          _Vertexes,
                std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes);
        };

        // RenderingQueue2D
        class RenderingQueue2D : public RenderingQueue
        {
        public:
            // nested types
            struct DefaultSymbolProcessor
            {
                void operator()(
                    const gs_2dboxf&    _CurrentSymbolBoundingBox,
                    const gs_vec2f&     _CursorPosition,
                    const int&          _Utf8IteratorPosition,
                    const unsigned int& _Symbol) const
                {
                    (void)_CurrentSymbolBoundingBox;
                    (void)_CursorPosition;
                    (void)_Utf8IteratorPosition;
                    (void)_Symbol;
                }
            };

            struct DefaultSymbolChanger
            {
                unsigned int operator()(const unsigned int& _Symbol) const
                {
                    return _Symbol;
                }
            };

            RenderingQueue2D();
            virtual ~RenderingQueue2D();

            // 2D API
            gs_vec2f calculate_arc_point(
                const gs_vec2f& _Center,
                const float&    _MinorRadius,
                const float&    _MajorRadius,
                const float&    _ArcAngle);

            template<typename Type, typename ProcessSymbol = DefaultSymbolProcessor, typename ChangeSymbol = DefaultSymbolChanger>
            gs_2dboxf calculate_bounding_box(
                const Type&                            _Begin,
                const Type&                            _End,
                const float&                           _Size,
                const ApplicationRenderingBackendFont& _Font,
                const ChangeSymbol&                    _ChangeSymbol = DefaultSymbolChanger())
            {
                gs_2dboxf textBoundingBox = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f));

                push_text(
                    gs_vec2f(0.f, 0.f),
                    _Begin,
                    _End,
                    _Size,
                    1,
                    gs_mat4f(1.f),
                    _Font,
                    true,
                    [&textBoundingBox](
                        const gs_2dboxf&    _CurrentSymbolBoundingBox,
                        const gs_vec2f&     _CursorPosition,
                        const int&          _Utf8IteratorPosition,
                        const unsigned int& _Symbol)
                    {
                        (void)_CurrentSymbolBoundingBox;
                        (void)_CursorPosition;
                        (void)_Utf8IteratorPosition;
                        (void)_Symbol;

                        // calculate text bounding box
                        textBoundingBox = gs_2dboxf(textBoundingBox.Min, _CurrentSymbolBoundingBox.Min, textBoundingBox.Max, _CurrentSymbolBoundingBox.Max);
                    },
                    _ChangeSymbol);

                return textBoundingBox;
            }

            template<typename Type, typename ProcessSymbol = DefaultSymbolProcessor, typename ChangeSymbol = DefaultSymbolChanger>
            gs_2dboxf calculate_bounding_box(
                const Type&                            _Begin,
                const Type&                            _End,
                const int&                             _SymbolsCount,
                const float&                           _Size,
                const ApplicationRenderingBackendFont& _Font,
                const ChangeSymbol&                    _ChangeSymbol = DefaultSymbolChanger())
            {
                gs_2dboxf textBoundingBox = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f));

                push_text_wrapped(
                    gs_vec2f(0.f, 0.f),
                    _Begin,
                    _End,
                    _SymbolsCount,
                    _Size,
                    1,
                    gs_mat4f(1.f),
                    _Font,
                    true,
                    [&textBoundingBox](
                        const gs_2dboxf&    _CurrentSymbolBoundingBox,
                        const gs_vec2f&     _CursorPosition,
                        const int&          _Utf8IteratorPosition,
                        const unsigned int& _Symbol)
                    {
                        (void)_CurrentSymbolBoundingBox;
                        (void)_CursorPosition;
                        (void)_Utf8IteratorPosition;
                        (void)_Symbol;

                        // calculate text bounding box
                        textBoundingBox = gs_2dboxf(textBoundingBox.Min, _CurrentSymbolBoundingBox.Min, textBoundingBox.Max, _CurrentSymbolBoundingBox.Max);
                    },
                    _ChangeSymbol);

                return textBoundingBox;
            }

            void push_triangle_filled(
                const gs_vec2f&                           _P1,
                const gs_vec2f&                           _P2,
                const gs_vec2f&                           _P3,
                const gs_color&                           _Color,
                const gs_mat4f&                           _Transform = gs_mat4f(1.f),
                const ApplicationRenderingBackendTexture& _Texture   = ApplicationRenderingBackendTexture());

            void push_rectangle_filled(
                const gs_vec2f&                           _Min,
                const gs_vec2f&                           _Max,
                const gs_color&                           _Color,
                const gs_mat4f&                           _Transform = gs_mat4f(1.f),
                const ApplicationRenderingBackendTexture& _Texture   = ApplicationRenderingBackendTexture());

            void push_rectangle_gradient_mesh(
                const gs_vec2f&  _Min,
                const gs_vec2f&  _Max,
                const gs_color& _Color1,
                const gs_color& _Color2,
                const gs_color& _Color3,
                const gs_color& _Color4,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            void push_rectangle_rounded_filled(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Radius,
                const gs_color& _Color,
                const gs_mat4f& _Transform              = gs_mat4f(1.f),
                bool            _RoundTopLeftCorner     = true,
                bool            _RoundTopRightCorner    = true,
                bool            _RoundBottomRightCorner = true,
                bool            _RoundBottomLeftCorner  = true);

            template<typename Type, typename ProcessSymbol = DefaultSymbolProcessor, typename ChangeSymbol = DefaultSymbolChanger>
            void push_text(
                const gs_vec2f&                        _Position,
                const Type&                            _Begin,
                const Type&                            _End,
                const float&                           _Size,
                const gs_color&                        _Color,
                const gs_mat4f&                        _Transform     = gs_mat4f(1.f),
                const ApplicationRenderingBackendFont& _Font          = ApplicationRenderingBackendFont(),
                const bool&                            _DoNotRender   = false,
                const ProcessSymbol&                   _ProcessSymbol = DefaultSymbolProcessor(),
                const ChangeSymbol&                    _ChangeSymbol  = DefaultSymbolChanger())
            {
                // main code
                ApplicationRenderingBackendFont font = _Font.is_null() ? ApplicationRenderingBackend::get_default_font() : _Font;

                float scale     = _Size / (float)font.SizeInPixels;
                float offset    = (font.Ascent + font.Descent + font.LineGap) * scale;
                float positionX = _Position.x;
                float positionY = _Position.y + gs_vec2f(0.f, offset).y;
                Type  start     = _Begin;
                Type  end       = _End;

                gs_2dboxf symbolBox = gs_2dboxf(gs_vec2f(positionX, positionY), gs_vec2f(positionX, positionY));

                while (start < end)                
                {
                    int cursor = (int)(start - _Begin);

                    unsigned int symbol = _ChangeSymbol(Frenchie::Core::String::utf8_next(start));

                    // fallbacks
                    if(!font.contains_glyph(symbol))
                    {
                        _ProcessSymbol(
                            gs_2dboxf(gs_vec2f(positionX, positionY), gs_vec2f(positionX, positionY)),
                            gs_vec2f(symbolBox.Max.x, positionY - offset),
                            cursor,
                            symbol);

                        // next line
                        if(symbol == '\n')
                        {
                            positionY += gs_vec2f(0.f, gs_max(_Size, gs_abs(offset))).y;
                            positionX =  _Position.x;
                        }
                        // carriage return
                        else if(symbol == '\r')
                        {
                            positionX =  _Position.x;
                        }
                        // tab
                        else if(symbol == '\t')
                        {
                            positionX += gs_vec2f(_Size, 0.f).x;
                        }
                        else
                        {
                            // TODO: do someting here...
                            // May be use fallback font and take fallback character from there ???
                        }

                        symbolBox = gs_2dboxf(
                            gs_vec2f(positionX, positionY) - gs_vec2f(0.f, offset),
                            gs_vec2f(positionX, positionY) - gs_vec2f(0.f, offset));

                        continue;
                    }

                    // render symbol mesh
                    ApplicationRenderingBackendGlyph glyph                  = font.retrieve_glyph(symbol);
                    float                            glyphWidth             = glyph.Box.size().x * scale;
                    float                            glyphHeight            = glyph.Box.size().y * scale;
                    float                            glyphHorizontalBearing = glyph.Bearing.x * scale;
                    float                            glyphVerticalBearing   = glyph.Bearing.y * scale;
                    float                            glyphAdvance           = glyph.Advance * scale;
                    gs_vec2f                         min                    = gs_vec2f(positionX, positionY) + gs_vec2f(glyphHorizontalBearing, glyphVerticalBearing);
                    gs_vec2f                         max                    = min + gs_vec2f(glyphWidth, glyphHeight);

                    if(!_DoNotRender)
                        build_rectangle_filled_mesh(min, max, glyph.MinUV, glyph.MaxUV, _Color);

                    // calculate last symbol bounding box
                    symbolBox = gs_2dboxf(min, max);
                    if(gs_vector_length(symbolBox.size()) <= 0.f)
                        symbolBox = gs_2dboxf(min - gs_vec2f(0.f, offset), min + gs_vec2f(glyphAdvance, 0.f));

                    // process symbol
                    _ProcessSymbol(symbolBox, gs_vec2f(min.x, positionY - offset), cursor, symbol);

                    // move cursor
                    positionX += gs_vec2f(glyphAdvance, 0.f).x;
                }

                // process last symbol
                _ProcessSymbol(
                    gs_2dboxf(gs_vec2f(positionX, positionY), gs_vec2f(positionX, positionY)),
                    gs_vec2f(positionX, positionY) - gs_vec2f(0.f, offset),
                    (int)(start - _Begin),
                    '\0');

                if(!_DoNotRender)
                    push_rendering_command(font.AtlasTexture, _Color, _Transform);
            }

            template<typename Type, typename ProcessSymbol = DefaultSymbolProcessor, typename ChangeSymbol = DefaultSymbolChanger>
            void push_text_wrapped(
                const gs_vec2f&                        _Position,
                const Type&                            _Begin,
                const Type&                            _End,
                const int&                             _SymbolsCount,
                const float&                           _Size,
                const gs_color&                        _Color,
                const gs_mat4f&                        _Transform     = gs_mat4f(1.f),
                const ApplicationRenderingBackendFont& _Font          = ApplicationRenderingBackendFont(),
                const bool&                            _DoNotRender   = false,
                const ProcessSymbol&                   _ProcessSymbol = DefaultSymbolProcessor(),
                const ChangeSymbol&                    _ChangeSymbol  = DefaultSymbolChanger())
            {
                // render default text
                gs_vec2f position = _Position;

                // render a part of an input text
                push_text(
                    _Position,
                    _Begin,
                    _Begin + gs_min<int>(_SymbolsCount, (int)(_End - _Begin)),
                    _Size,
                    _Color,
                    _Transform,
                    _Font,
                    _DoNotRender,
                    [&_ProcessSymbol, &position](
                       const gs_2dboxf&    _CurrentSymbolBoundingBox,
                       const gs_vec2f&     _CursorPosition,
                       const int&          _Utf8IteratorPosition,
                       const unsigned int& _Symbol)
                    {
                        _ProcessSymbol(_CurrentSymbolBoundingBox, _CursorPosition, _Utf8IteratorPosition, _Symbol);
                        position = _CursorPosition;
                    },
                    _ChangeSymbol);

                // render points
                if(_SymbolsCount < (int)(_End - _Begin))
                {
                    char text[4] = "...";

                    push_text(
                        position,
                        &text[0],
                        &text[0] + 3,
                        _Size,
                        _Color,
                        _Transform,
                        _Font,
                        _DoNotRender,
                        _ProcessSymbol,
                        _ChangeSymbol);
                }
            }

            void push_arc_filled(
                const gs_vec2f&                           _Center,
                const float&                              _MinorRadius,
                const float&                              _MajorRadius,
                const float&                              _SourceAngle,
                const float&                              _TargetAngle,
                const gs_color&                           _Color,
                const gs_mat4f&                           _Transform = gs_mat4f(1.f),
                const ApplicationRenderingBackendTexture& _Texture   = ApplicationRenderingBackendTexture());

            void push_line(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const float&    _Width,
                const gs_color& _Color,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            void push_arc(
                const gs_vec2f& _Center,
                const float&    _MinorRadius,
                const float&    _MajorRadius,
                const float&    _SourceAngle,
                const float&    _TargetAngle,
                const float&    _Width,
                const gs_color& _Color,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            void push_triangle(
                const gs_vec2f&  _P1,
                const gs_vec2f&  _P2,
                const gs_vec2f&  _P3,
                const float&     _Width,
                const gs_color&  _Color,
                const gs_mat4f&  _Transform = gs_mat4f(1.f));

            void push_rectangle(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Width,
                const gs_color& _Color,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            void push_rectangle_rounded(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Radius,
                const float&    _Width,
                const gs_color& _Color,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            // auxiliary mesh building API
            void build_triangle_filled_mesh(
                const gs_vec2f&                           _P1,
                const gs_vec2f&                           _P2,
                const gs_vec2f&                           _P3,
                const gs_color&                           _Color,
                const ApplicationRenderingBackendTexture& _Texture);

            void build_triangle_gradient_mesh(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const gs_vec2f& _P3,
                const gs_color& _Color1,
                const gs_color& _Color2,
                const gs_color& _Color3);

            void build_rectangle_filled_mesh(
                const gs_vec2f&                           _Min,
                const gs_vec2f&                           _Max,
                const gs_color&                           _Color,
                const ApplicationRenderingBackendTexture& _Texture);

            void build_rectangle_filled_mesh(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const gs_vec2f& _MinUV,
                const gs_vec2f& _MaxUV,
                const gs_color& _Color);

            void build_rectangle_gradient_mesh(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const gs_color& _Color1,
                const gs_color& _Color2,
                const gs_color& _Color3,
                const gs_color& _Color4);

            void build_arc_filled_mesh(
                const gs_vec2f&                           _Center,
                const float&                              _MinorRadius,
                const float&                              _MajorRadius,
                const float&                              _SourceAngle,
                const float&                              _TargetAngle,
                const gs_color&                           _Color,
                const ApplicationRenderingBackendTexture& _Texture,
                const int&                                _SegmentsCount = 36);

            void build_line_mesh(
                const gs_vec2f&                           _P1,
                const gs_vec2f&                           _P2,
                const float&                              _Width,
                const gs_color&                           _Color,
                const ApplicationRenderingBackendTexture& _Texture);

            void build_arc_mesh(
                const gs_vec2f& _Center,
                const float&    _MinorRadius,
                const float&    _MajorRadius,
                const float&    _SourceAngle,
                const float&    _TargetAngle,
                const float&    _Width,
                const gs_color& _Color,
                const int&      _SegmentsCount = 36);

        protected:

            RenderingQueue2DPathBuilder  m_PathBuilder {RenderingQueue2DPathBuilder(8.f)};
        };
    };
}