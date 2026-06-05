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
             * @brief This function calculates 2D transform matrix.
             * @param _Depth depth
             * @param _Position translate position
             * @param _Rotation 2D rotation XY vector
             * @param _Scale 2D scale XY vector
             * @return 
             */
            gs_mat4f calculate_transform_matrix(
                const float&    _Depth,
                const gs_vec2f& _Position = gs_vec2f(0.f, 0.f),
                const float&    _Rotation = 0.f,
                const gs_vec2f& _Scale    = gs_vec2f(1.f, 1.f));

            /**
             * @brief Builds filled polygon mesh
             * @param _Points polygon points
             * @param _Colors polygon points colors
             * @param _Count number of polygon points
             * @details This function builds polygon out-of points array using ear clipping algorithm.
             */
            void build_poly_mesh_filled(const gs_vec2f _Points[], const gs_color _Colors[], gs_vec2f _UVs[], const int& _Count);

            /**
             * @brief Builds line mesh
             * @param _P1 line source point
             * @param _P2 line target point
             * @param _Width line width
             * @param _Color line fill color
             */
            void build_line_mesh(const gs_vec2f& _P1, const gs_vec2f& _P2, const float& _Width, const gs_color& _Color);

            /**
             * @brief Builds triangle filled with color mesh
             * @param _P1 first point
             * @param _P2 second point
             * @param _P3 third point
             * @param _Color color
             */
            void build_triangle_filled_mesh(const gs_vec2f& _P1, const gs_vec2f& _P2, const gs_vec2f& _P3, const gs_color& _Color);

            /**
             * @brief Builds triangle mesh
             * @param _P1 first point
             * @param _P2 second point
             * @param _P3 third point
             * @param _Width line width
             * @param _Color color
             */
            void build_triangle_mesh(const gs_vec2f& _P1, const gs_vec2f& _P2, const gs_vec2f& _P3, const float& _Width, const gs_color& _Color);

            /**
             * @brief Builds rectangle filled with color mesh
             * @param _Min top left point
             * @param _Max bottom right point
             * @param _Color fill color
             * @param _Radius corner rounding radius
             */
            void build_rectangle_filled_mesh(const gs_vec2f& _Min, const gs_vec2f& _Max, const gs_color& _Color, const float& _Radius = 0.f);

            /**
             * @brief Builds rectangle filled with color mesh
             * @param _Min top left point
             * @param _Max bottom right point
             * @param _Color fill color
             * @param _Width line width
             * @param _Radius corner rounding radius
             */
            void build_rectangle_mesh(const gs_vec2f& _Min, const gs_vec2f& _Max, const gs_color& _Color, const float& _Width, const float& _Radius = 0.f);

            /**
             * @brief Build arc filled with color mesh
             * @param _Center arc center
             * @param _MinorRadius arc minor radius
             * @param _MajorRadius arc major radius
             * @param _SourceAngle arc source angle
             * @param _TargetAngle arc target angle
             * @param _Color arc fill color
             * @param _SegmentsCount arc segments count
             */
            void build_arc_filled_mesh(
                const gs_vec2f& _Center,
                const float&    _MinorRadius,
                const float&    _MajorRadius,
                const float&    _SourceAngle,
                const float&    _TargetAngle,
                const gs_color& _Color);

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
             */
            void build_arc_mesh(
                const gs_vec2f& _Center,
                const float&    _MinorRadius,
                const float&    _MajorRadius,
                const float&    _SourceAngle,
                const float&    _TargetAngle,
                const float&    _Width,
                const gs_color& _Color);

            /**
             * @brief Builds polygon mesh
             * @param _Points polygon points
             * @param _Color polygon lines color
             * @param _Count number of polygon points
             * @param _Width line width
             * @details This function builds polygon out-of points array.
             */
            void build_poly_mesh(
                const gs_vec2f  _Points[],
                const gs_color  _Color,
                const int&      _Count,
                const float&    _Width);

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
             * @brief Renders filled rectangle
             * @param _Min top left
             * @param _Max bottom right
             * @param _Color fill color
             * @param _Transform 2D transform matrix
             * @param _Texture mesh texture
             */
            void push_rectangle_filled(
                const gs_vec2f&                           _Min,
                const gs_vec2f&                           _Max,
                const gs_color&                           _Color,
                const gs_mat4f&                           _Transform = gs_mat4f(1.f),
                const float&                              _Radius    = 0.f,
                const ApplicationRenderingBackendTexture& _Texture   = ApplicationRenderingBackendTexture());

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
             * @brief Builds filled polygon mesh
             * @param _Points polygon points
             * @param _Colors polygon points colors
             * @param _Count number of polygon points
             * @param _Transform transform matrix
             * @param _Texture polygon texture
             * @details This function builds polygon out-of points array using ear clipping algorithm.
             */
            void push_poly_filled(
                const gs_vec2f                            _Points[],
                const gs_color                            _Colors[],
                const int&                                _Count,
                const gs_mat4f&                           _Transform = gs_mat4f(1.f),
                const ApplicationRenderingBackendTexture& _Texture   = ApplicationRenderingBackendTexture());

            struct Edge
            {
                gs_vec2f P1;
                gs_vec2f P2;
            };

            struct Triangle
            {
                gs_vec2f P1;
                gs_vec2f P2;
                gs_vec2f P3;
            };

            std::vector<Triangle> push_poly_filled_Delaunay(
                const gs_vec2f                            _Points[],
                const gs_color                            _Colors[],
                const int&                                _Count,
                const gs_mat4f&                           _Transform = gs_mat4f(1.f),
                const ApplicationRenderingBackendTexture& _Texture   = ApplicationRenderingBackendTexture());

            /**
             * @brief Renders triangle
             * @param _P1 first point
             * @param _P2 second point
             * @param _P3 third point
             * @param _Color color
             * @param _Width line width
             * @param _Transform 2D transform matrix
             */
            void push_triangle(
                const gs_vec2f&  _P1,
                const gs_vec2f&  _P2,
                const gs_vec2f&  _P3,
                const gs_color&  _Color,
                const float&     _Width,
                const gs_mat4f&  _Transform = gs_mat4f(1.f));

            /**
             * @brief Renders rectangle
             * @param _Min top left
             * @param _Max bottom right
             * @param _Color color
             * @param _Width line width
             * @param _Transform 2D transform matrix
             * @param _Radius corners rounding radius
             */
            void push_rectangle(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const gs_color& _Color,
                const float&    _Width,
                const gs_mat4f& _Transform = gs_mat4f(1.f),
                const float&    _Radius    = 0.f);

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
                const gs_color& _Color,
                const float&    _Width,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            /**
             * @brief Renders polygon mesh
             * @param _Points polygon points
             * @param _Colors polygon lines color
             * @param _Count number of polygon points
             * @param _Width line width
             * @param _Transform transform matrix
             * @details This function builds polygon out-of points array.
             */
            void push_poly(
                const gs_vec2f  _Points[],
                const gs_color  _Color,
                const int&      _Count,
                const float&    _Width,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

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
                    {
                        gs_vec2f points[4] = { gs_vec2f(min.x, min.y), gs_vec2f(max.x, min.y), gs_vec2f(max.x, max.y), gs_vec2f(min.x, max.y) };
                        gs_color colors[4] = { _Color, _Color, _Color, _Color };
                        gs_vec2f uvs   [4] = { gs_vec2f(glyph.MinUV.x, glyph.MinUV.y), gs_vec2f(glyph.MaxUV.x, glyph.MinUV.y), gs_vec2f(glyph.MaxUV.x, glyph.MaxUV.y), gs_vec2f(glyph.MinUV.x, glyph.MaxUV.y) };

                        build_poly_mesh_filled(points, colors, uvs, 4);
                    }

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
             * @brief Renders wrapped text
             * @param _Position rendered text position 
             * @param _Begin text start interator
             * @param _End text end iterator
             * @param _BoundingBox bounding box that should encapsulate text
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
                const gs_2dboxf                        _BoundingBox,
                const float&                           _Size,
                const gs_color&                        _Color,
                const gs_mat4f&                        _Transform     = gs_mat4f(1.f),
                const ApplicationRenderingBackendFont& _Font          = ApplicationRenderingBackendFont(),
                const bool&                            _DoNotRender   = false,
                const ProcessSymbol&                   _ProcessSymbol = DefaultSymbolProcessor(),
                const ChangeSymbol&                    _ChangeSymbol  = DefaultSymbolChanger())
            {
                for (int symbolsCount = (int)(_End - _Begin); symbolsCount > 0; --symbolsCount)
                {
                    gs_2dboxf boundingBox = calculate_bounding_box(_Begin, _End, symbolsCount, _Size, _Font, _ChangeSymbol);

                    if(boundingBox.width() > _BoundingBox.width() || boundingBox.height() > _BoundingBox.height())
                        continue;

                    push_text_wrapped(_Position, _Begin, _End, symbolsCount, _Size, _Color, _Transform, _Font, _DoNotRender, _ProcessSymbol, _ChangeSymbol);
                    return;
                }
            }

        protected:

            std::vector<int> m_TriangulationIndexes{std::vector<int>()};
        };

        /*! @} */
    };
}