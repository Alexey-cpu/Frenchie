#pragma once

// Core
#include <FrenchieCoreMath.hpp>
#include <FrenchieCoreClock.hpp>
#include <FrenchieCoreOptional.hpp>
#include <FrenchieCoreRingBuffer.hpp>

// Core
#include <FrenchieCoreStringUtilities.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationPlatformBackend.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>

// STL
#include <iostream>

namespace Frenchie
{
    namespace Application
    {
        struct RenderingQueuePathSegment
        {
        public:
            RenderingQueuePathSegment(const gs_vec2f& _P1, const gs_vec2f& _P2, const float& _Width = 4.f);
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

        struct RenderingQueuePathBuilder
        {
        public:

            RenderingQueuePathBuilder(const float& _PolygonLinesWidth);
            ~RenderingQueuePathBuilder();

            // TODO: add Bezier and random power curves here e.t.c
            void begin(const gs_vec2f& _Point);

            void line_to(const gs_vec2f& _Target);
            void arc_to(const gs_vec2f& _Target, const float& _Radius);

            void build_mesh(
                const gs_color&                                          _Color,
                const float&                                             _Width,
                std::vector<ApplicationRenderingBackendVertex>&          _Vertexes,
                std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes,
                const ApplicationRenderingBackendTexture&                _Texture);

            void build_mesh_filled(
                const gs_color&                                          _Color,
                std::vector<ApplicationRenderingBackendVertex>&          _Vertexes,
                std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes,
                const ApplicationRenderingBackendTexture&                _Texture);

        protected:

            std::vector<RenderingQueuePathSegment> m_PolygonLines           {std::vector<RenderingQueuePathSegment>()};
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
                std::vector<ApplicationRenderingBackendVertex>&          _Vertexes,
                std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes);
        };

        struct RenderingQueueMesh final
        {
            RenderingQueueMesh(
                const int _VertexesCount  = -1,
                const int _VertexesOffset = -1,
                const int _IndexesCount   = -1,
                const int _IndexesOffset  = -1) :
                VertexesCount(_VertexesCount),
                VertexesOffset(_VertexesOffset),
                IndexesCount(_IndexesCount),
                IndexesOffset(_IndexesOffset){}

            int VertexesOffset;                           
            int VertexesCount;
            int IndexesOffset;
            int IndexesCount;
        };

        struct RenderingQueueRenderingCommand final
        {
            RenderingQueueRenderingCommand(
                const RenderingQueueMesh&                                   _Mesh,
                const ApplicationRenderingBackendTexture&                   _Texture,
                const gs_mat4f&                                             _Transform,
                const ApplicationRenderingBackendGraphicsApiRenderingHints& _MeshRenderingHints) :
                Mesh(_Mesh),
                Texture(_Texture),
                Transform(_Transform),
                MeshRendererHints(_MeshRenderingHints){}

            RenderingQueueMesh                                   Mesh             {RenderingQueueMesh()};
            ApplicationRenderingBackendTexture                   Texture          {ApplicationRenderingBackendTexture()};
            gs_mat4f                                             Transform        {gs_mat4f(1.f)};
            ApplicationRenderingBackendGraphicsApiRenderingHints MeshRendererHints{ApplicationRenderingBackendGraphicsApiRenderingHints_::ApplicationRenderingBackendGraphicsApiRenderingHints_Default};
        };

        struct RenderingQueueRendererCommandClearColor final
        {
            RenderingQueueRendererCommandClearColor(
                const gs_color& _ClearColor) : ClearColor(_ClearColor){}
            gs_color ClearColor;
        };

        struct RenderingQueueRendererCommandClippingBox final
        {
            RenderingQueueRendererCommandClippingBox(
                const gs_2dboxf& _ClippinBox) : ClippingBox(_ClippinBox){}

            gs_2dboxf ClippingBox;
        };

        struct RenderingQueueCommand final
        {
            RenderingQueueCommand(
                const RenderingQueueRenderingCommand&           _Command,
                const RenderingQueueRendererCommandClearColor&  _ClearColor,
                const RenderingQueueRendererCommandClippingBox& _ClippingBox) :
                Command(_Command),
                ClearColor(_ClearColor),
                ClippingBox(_ClippingBox){}

            Frenchie::Core::Optional<RenderingQueueRenderingCommand>           Command;
            Frenchie::Core::Optional<RenderingQueueRendererCommandClearColor>  ClearColor;
            Frenchie::Core::Optional<RenderingQueueRendererCommandClippingBox> ClippingBox;
        };

        struct RenderingQueueMetrics final
        {
            double FrameRate              = 0.0;
            int    RenderedTrianglesCount = 0;
            int    RenderingCommandsCount = 0;
        };

        class RenderingQueue : public Layer
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

            // constructors
            RenderingQueue();

            // destructor
            virtual ~RenderingQueue();

            // getters
            RenderingQueueMetrics get_rendering_queue_metrics() const;
            float                 get_minimum_line_width() const;
            gs_vec3f              get_cursor_postion() const;
            float                 get_near_plane() const;
            float                 get_far_plane() const;

            // Layer API
            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_render() override;
            virtual void frame_finish() override;
            virtual void finish() override;
            virtual void quit() override;
            virtual bool allows_multiple_instances() const override;

            // API
            void push_clip_box(
                const gs_2dboxf& _Value,
                const gs_mat4f&  _Transform = gs_mat4f(1.f));
            void pop_clip_box();

            void push_clear_color(const gs_color& _Value);
            void pop_clear_color();
            
            gs_2dboxf current_viewport() const;
            gs_2dboxf current_clipping_box() const;
            gs_color  current_clear_color() const;

            gs_mat4f calculate_transform_matrix(
                const float&    _Depth,
                const gs_vec2f& _Position = gs_vec2f(0.f, 0.f),
                const float&    _Rotation = 0.f,
                const gs_vec2f& _Scale    = gs_vec2f(1.f, 1.f));

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

            // commands API
            void push_rendering_command(
                const gs_mat4f&                                             _Transform,
                const ApplicationRenderingBackendGraphicsApiRenderingHints& _MeshRenderingHints = ApplicationRenderingBackendGraphicsApiRenderingHints_::ApplicationRenderingBackendGraphicsApiRenderingHints_Default);

            void push_rendering_command(
                const ApplicationRenderingBackendTexture&                   _Texture,
                const gs_color&                                             _Color,
                const gs_mat4f&                                             _Transform,
                const ApplicationRenderingBackendGraphicsApiRenderingHints& _MeshRenderingHints = ApplicationRenderingBackendGraphicsApiRenderingHints_::ApplicationRenderingBackendGraphicsApiRenderingHints_Default);

            void push_rendering_command(
                const RenderingQueueMesh&                                   _Mesh,
                const ApplicationRenderingBackendTexture&                   _Texture,
                const gs_mat4f&                                             _Transform,
                const ApplicationRenderingBackendGraphicsApiRenderingHints& _RendererHints,
                const gs_color&                                             _ClearColor,
                const gs_2dboxf&                                            _ClippinBox);

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

            // rendering queue data
            gs_2dboxf                                               m_Viewport                           {gs_vec2f(-gs_huge<float>(), -gs_huge<float>()), gs_vec2f(+gs_huge<float>(), +gs_huge<float>())};
            std::vector<gs_color>                                   m_ClearColors                        {std::vector<gs_color>()};
            std::vector<gs_2dboxf>                                  m_ClippingBoxes                      {std::vector<gs_2dboxf>()};
            std::vector<ApplicationRenderingBackendVertex>          m_MeshVertexes                       {std::vector<ApplicationRenderingBackendVertex>()};
            std::vector<ApplicationRenderingBackendMeshVertexIndex> m_MeshVertexesIndexes                {std::vector<ApplicationRenderingBackendMeshVertexIndex>()};
            float                                                   m_MinimumLineWidth                   {4.f};

            // path building data
            RenderingQueuePathBuilder                               m_PathBuilder                        {RenderingQueuePathBuilder(8.f)};

            // rendering
            gs_mat4f                                                m_ProjectionMatrix                   {gs_mat4f(1)};
            gs_mat4f                                                m_CameraViewMatrix                   {gs_mat4f(1)};
            std::vector<RenderingQueueCommand>                      m_Commands                           {std::vector<RenderingQueueCommand>()};

            // metrics measurement
            std::chrono::high_resolution_clock::time_point          m_FrameRateMeasurementStartTimePoint {Frenchie::Core::Clock::tic()};
            Frenchie::Core::RingBuffer<double, 64>                  m_FrameRateMeasurementFilterBuffer   {Frenchie::Core::RingBuffer<double, 64>(0.0)};
            RenderingQueueMetrics                                   m_Metrics                            {RenderingQueueMetrics()};

            ApplicationRenderingBackendMeshVertexIndex              m_IndexesOffset                      {0};
            ApplicationRenderingBackendMeshVertexIndex              m_VertexesOffset                     {0};
        };
    }
}
