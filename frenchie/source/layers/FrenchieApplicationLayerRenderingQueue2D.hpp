#pragma once

#include <FrenchieApplicationLayerRenderingQueue.hpp>

/*! \defgroup <ApplicationLayers> (Application layers)
*  @brief The module contains main application layers.
    @{
*/

/*! @} */

namespace Frenchie
{
    namespace Application
    {
        /*! \defgroup <ApplicationRenderingQueue2D> (Application 2D rendering queue)
        *  @ingroup ApplicationLayers
        *  @brief The module contains utility clases used for 2D rendering.
        *  @{
        */

        /**
         * @brief This struct represents 2D line path segment.
         * @struct RenderingQueue2DPathSegment
         */
        struct RenderingQueue2DPathSegment
        {
        public:

            /**
             * @brief Constructs a new 2D line path segment.
             * @param _P1 source line point
             * @param _P2 target line point
             * @param _Width line width
             */
            RenderingQueue2DPathSegment(const gs_vec2f& _P1, const gs_vec2f& _P2, const float& _Width = 4.f);
            
            /**
             * @brief This function recalculates 2D line path segment parameters
             * @param _P1 
             * @param _P2
             * @param _Width 
             */
            void setup(const gs_vec2f& _P1, const gs_vec2f& _P2, const float& _Width);
            
            /**
             * @brief This function recalculates 2D line path segment parameters
             * @param _P1 
             * @param _P2
             * @param _Width 
             */
            void setup(const float& _Width);

            gs_vec2f P1;
            gs_vec2f P1min;
            gs_vec2f P1max;

            gs_vec2f P2;
            gs_vec2f P2min;
            gs_vec2f P2max;

            int      Index{0};
        };

        /**
         * @brief This struct represents 2D line path builder.
         * @struct RenderingQueue2DPathBuilder
         */
        struct RenderingQueue2DPathBuilder
        {
        public:

            /**
             * @brief Initializes a new 2D line path builder.
             * @param _PolygonLinesWidth line path builder polygon lines width 
             */
            RenderingQueue2DPathBuilder(const float& _PolygonLinesWidth);
            ~RenderingQueue2DPathBuilder();

            // TODO: add Bezier and random power curves here e.t.c

            /**
             * @brief This function starts a new path from a given point
             * @param _Point path starting point 
             */
            void begin(const gs_vec2f& _Point);

            /**
             * @brief This function connects previous path point with provided path point by a line
             * @param _Target target point to which generated line is connected 
             */
            void line_to(const gs_vec2f& _Target);

            /**
             * @brief This function connects previous path point with provided path point by an arc of a given radius.
             * @param _Target target point to which generated arc is connected 
             * @param _Radius generated arc radius 
             */
            void arc_to(const gs_vec2f& _Target, const float& _Radius);

            /**
             * @brief This function builds lines mesh for generated path and clears path utility arrays.
             * @param _Color mesh vertexes color
             * @param _Width mesh lines width
             * @param _Vertexes mesh vertexes array filled by a function
             * @param _Indexes mesh indexes array filled by a function
             * @param _Texture mesh texture 
             */
            void build_mesh(
                const gs_color&                                          _Color,
                const float&                                             _Width,
                std::vector<ApplicationRenderingBackendMeshVertex>&      _Vertexes,
                std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes,
                const ApplicationRenderingBackendTexture&                _Texture);

            /**
             * @brief This function builds filled mesh for generated path and clears path utility arrays.
             * @param _Color mesh vertexes color
             * @param _Vertexes mesh vertexes array filled by a function
             * @param _Indexes mesh indexes array filled by a function
             * @param _Texture mesh texture 
             */
            void build_mesh_filled(
                const gs_color&                                          _Color,
                std::vector<ApplicationRenderingBackendMeshVertex>&      _Vertexes,
                std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes,
                const ApplicationRenderingBackendTexture&                _Texture);

        protected:

            std::vector<RenderingQueue2DPathSegment> m_PolygonLines         {std::vector<RenderingQueue2DPathSegment>()};
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
                std::vector<ApplicationRenderingBackendMeshVertex>&      _Vertexes,
                std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes);
        };

        /**
         * @brief This class encapsulates functionality for 2D rendering.
         * @class RenderingQueue2D
         */
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

            /**
             * @brief Calculates bouinding box of input text
             * @param _Begin input string start begin iterator
             * @param _End input string start end iterator
             * @param _Size text size in pixels
             * @param _Font font
             * @param _ChangeSymbol symbol changer lambda
             * @return returns input text bounding box 
             */
            template<typename Type, typename ChangeSymbol = DefaultSymbolChanger>
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

            /**
             * @brief Calculates bouinding box of input wrapped text
             * @param _Begin input string start begin iterator
             * @param _End input string start end iterator
             * @param _SymbolsCount number of displayed symbols
             * @param _Size text size in pixels
             * @param _Font font
             * @param _ChangeSymbol symbol changer lambda
             * @return returns input text bounding box 
             */
            template<typename Type, typename ChangeSymbol = DefaultSymbolChanger>
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

            /**
             * @brief Renders triangle filled with color
             * @param _P1 first point
             * @param _P2 second point
             * @param _P3 third point
             * @param _Color color
             * @param _Transform 2D transform matrix
             * @param _Texture mesh texture
             */
            void push_triangle_filled(
                const gs_vec2f&                           _P1,
                const gs_vec2f&                           _P2,
                const gs_vec2f&                           _P3,
                const gs_color&                           _Color,
                const gs_mat4f&                           _Transform = gs_mat4f(1.f),
                const ApplicationRenderingBackendTexture& _Texture   = ApplicationRenderingBackendTexture());

            /**
             * @brief Renders rectangle filled with color
             * @param _Min top left
             * @param _Max bottom right
             * @param _Color color
             * @param _Transform 2D transform matrix
             * @param _Texture mesh texture
             */
            void push_rectangle_filled(
                const gs_vec2f&                           _Min,
                const gs_vec2f&                           _Max,
                const gs_color&                           _Color,
                const gs_mat4f&                           _Transform = gs_mat4f(1.f),
                const ApplicationRenderingBackendTexture& _Texture   = ApplicationRenderingBackendTexture());

            /**
             * @brief Renders gradient rectangle
             * @param _Min top left
             * @param _Max bottom right
             * @param _Color1 color of top left point
             * @param _Color2 color of top right point
             * @param _Color3 color of bottom right point
             * @param _Color4 color of bottom left point
             * @param _Transform 2D transform matrix
             */
            void push_rectangle_gradient_mesh(
                const gs_vec2f&  _Min,
                const gs_vec2f&  _Max,
                const gs_color& _Color1,
                const gs_color& _Color2,
                const gs_color& _Color3,
                const gs_color& _Color4,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            /**
             * @brief Renders gradient rectangle
             * @param _Min top left
             * @param _Max bottom right
             * @param _Radius rounding radius
             * @param _Color color
             * @param _Transform 2D transform matrix
             * @param _RoundTopLeftCorner if true rounds top left corner
             * @param _RoundTopRightCorner if true rounds top right corner
             * @param _RoundBottomRightCorner if true rounds bottom right corner
             * @param _RoundBottomLeftCorner if true rounds bottom left corner
             */
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

            /**
             * @brief Renders text
             * @param _Position rendered text position 
             * @param _Begin text start interator
             * @param _End text end iterator
             * @param _Size text size
             * @param _Color text color
             * @param _Transform text 2D transform matrix
             * @param _Font font
             * @param _DoNotRender if true just processes text and does not render it
             * @param _ProcessSymbol symbol processor instance
             * @param _ChangeSymbol symbol changer instance
             */
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

            /**
             * @brief Renders wrapped text
             * @param _Position rendered text position 
             * @param _Begin text start interator
             * @param _End text end iterator
             * @param _SymbolsCount number of rendered symbols
             * @param _Size text size
             * @param _Color text color
             * @param _Transform text 2D transform matrix
             * @param _Font font
             * @param _DoNotRender if true just processes text and does not render it
             * @param _ProcessSymbol symbol processor instance
             * @param _ChangeSymbol symbol changer instance
             */
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

            /**
             * @brief Renders arc filled with color
             * @param _Center arc center
             * @param _MinorRadius arc minor radius
             * @param _MajorRadius arc major radius
             * @param _SourceAngle arc source angle
             * @param _TargetAngle arc target angle
             * @param _Color arc fill color
             * @param _Transform arc 2D transform matrix
             * @param _Texture arc mesh texture 
             */
            void push_arc_filled(
                const gs_vec2f&                           _Center,
                const float&                              _MinorRadius,
                const float&                              _MajorRadius,
                const float&                              _SourceAngle,
                const float&                              _TargetAngle,
                const gs_color&                           _Color,
                const gs_mat4f&                           _Transform = gs_mat4f(1.f),
                const ApplicationRenderingBackendTexture& _Texture   = ApplicationRenderingBackendTexture());

            /**
             * @brief Renders line
             * @param _P1 line source point
             * @param _P2 line target point
             * @param _Width line width
             * @param _Color line fill color
             * @param _Transform 2D transform matrix 
             */
            void push_line(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const float&    _Width,
                const gs_color& _Color,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            /**
             * @brief Renders arc
             * @param _Center arc center
             * @param _MinorRadius arc minor radius
             * @param _MajorRadius arc major radius
             * @param _SourceAngle arc source angle
             * @param _TargetAngle arc target angle
             * @param _Width arc line width
             * @param _Color arc fill color
             * @param _Transform arc 2D transform matrix
             */
            void push_arc(
                const gs_vec2f& _Center,
                const float&    _MinorRadius,
                const float&    _MajorRadius,
                const float&    _SourceAngle,
                const float&    _TargetAngle,
                const float&    _Width,
                const gs_color& _Color,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            /**
             * @brief Renders triangle
             * @param _P1 first point
             * @param _P1 second point
             * @param _P3 third point
             * @param _Width line width
             * @param _Color color
             * @param _Transform 2D transform matrix
             */
            void push_triangle(
                const gs_vec2f&  _P1,
                const gs_vec2f&  _P2,
                const gs_vec2f&  _P3,
                const float&     _Width,
                const gs_color&  _Color,
                const gs_mat4f&  _Transform = gs_mat4f(1.f));

            /**
             * @brief Renders rectangle
             * @param _Min top left
             * @param _Max bottom right
             * @param _Width line width
             * @param _Color color
             * @param _Transform 2D transform matrix
             */
            void push_rectangle(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Width,
                const gs_color& _Color,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            /**
             * @brief Renders rounded rectangle
             * @param _Min top left
             * @param _Max bottom right
             * @param _Radius rounding radius
             * @param _Width line width
             * @param _Color color
             * @param _Transform 2D transform matrix
             */
            void push_rectangle_rounded(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Radius,
                const float&    _Width,
                const gs_color& _Color,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            // auxiliary mesh building API
            /**
             * @brief Builds triangle filled with color mesh
             * @param _P1 first point
             * @param _P1 second point
             * @param _P3 third point
             * @param _Color color
             * @param _Texture mesh texture
             */
            void build_triangle_filled_mesh(
                const gs_vec2f&                           _P1,
                const gs_vec2f&                           _P2,
                const gs_vec2f&                           _P3,
                const gs_color&                           _Color,
                const ApplicationRenderingBackendTexture& _Texture);

            /**
             * @brief Builds triangle mesh filled with color
             * @param _P1 first point
             * @param _P1 second point
             * @param _P3 third point
             * @param _Color1 color of first point
             * @param _Color2 color of second point
             * @param _Color3 color of third point
             */
            void build_triangle_gradient_mesh(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const gs_vec2f& _P3,
                const gs_color& _Color1,
                const gs_color& _Color2,
                const gs_color& _Color3);

            /**
             * @brief Builds rectangle mesh filled with color
             * @param _Min top left
             * @param _Max bottom right
             * @param _Color color
             * @param _Texture mesh texture
             */
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

            /**
             * @brief Builds gradient rectangle mesh
             * @param _Min top left
             * @param _Max bottom right
             * @param _Color1 color of top left point
             * @param _Color2 color of top right point
             * @param _Color3 color of bottom right point
             * @param _Color4 color of bottom left point
             */
            void build_rectangle_gradient_mesh(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const gs_color& _Color1,
                const gs_color& _Color2,
                const gs_color& _Color3,
                const gs_color& _Color4);

            /**
             * @brief Build arc filled with color mesh
             * @param _Center arc center
             * @param _MinorRadius arc minor radius
             * @param _MajorRadius arc major radius
             * @param _SourceAngle arc source angle
             * @param _TargetAngle arc target angle
             * @param _Color arc fill color
             * @param _Texture arc mesh texture
             * @param _SegmentsCount arc segments count
             */
            void build_arc_filled_mesh(
                const gs_vec2f&                           _Center,
                const float&                              _MinorRadius,
                const float&                              _MajorRadius,
                const float&                              _SourceAngle,
                const float&                              _TargetAngle,
                const gs_color&                           _Color,
                const ApplicationRenderingBackendTexture& _Texture,
                const int&                                _SegmentsCount = 36);

            /**
             * @brief Renders line
             * @param _P1 line source point
             * @param _P2 line target point
             * @param _Width line width
             * @param _Color line fill color
             * @param _Texture mesh texture
             */
            void build_line_mesh(
                const gs_vec2f&                           _P1,
                const gs_vec2f&                           _P2,
                const float&                              _Width,
                const gs_color&                           _Color,
                const ApplicationRenderingBackendTexture& _Texture);

            /**
             * @brief Builds arc mesh
             * @param _Center arc center
             * @param _MinorRadius arc minor radius
             * @param _MajorRadius arc major radius
             * @param _SourceAngle arc source angle
             * @param _TargetAngle arc target angle
             * @param _Width arc line width
             * @param _Color arc fill color
             * @param _SegmentsCount arc segments count
             * @param [_SegmentsCount] arc segments count
             */
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

        /*! @} */
    };
}