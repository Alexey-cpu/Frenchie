#pragma once

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationLayerRenderingQueue.hpp>

// STB
#include "stb_image_write.h"
#include "stb_truetype.h"

namespace Frenchie
{
    namespace Application
    {
        struct Immediate2DRendererPathSegment
        {
        public:
            Immediate2DRendererPathSegment(const gs_vec2f& _P1, const gs_vec2f& _P2, const float& _Width = 4.f)
            {
                setup(_P1, _P2, _Width);
            }

            void setup(const gs_vec2f& _P1, const gs_vec2f& _P2, const float _Width)
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

            void setup(const float& _Width)
            {
                setup(P1, P2, _Width);
            }

            gs_vec2f P1;
            gs_vec2f P1min;
            gs_vec2f P1max;

            gs_vec2f P2;
            gs_vec2f P2min;
            gs_vec2f P2max;

            bool     Index{0};
        };

        struct Immediate2DRendererPathBuilder
        {
        public:

            Immediate2DRendererPathBuilder(const float& _PolygonLinesWidth = 4.f) : m_PolygonLinesWidth(_PolygonLinesWidth){}
            ~Immediate2DRendererPathBuilder(){}

            // TODO: add Bezier and random power curves here e.t.c
            void begin(const gs_vec2f& _Point)
            {
                m_PolygonLinesSourcePoint = _Point;
                m_PolygonLines.clear();
                m_PolygonLinesIndexes.clear();
            }

            void line_to(const gs_vec2f& _Target)
            {
                m_PolygonLines.push_back(Immediate2DRendererPathSegment(m_PolygonLinesSourcePoint, _Target, m_PolygonLinesWidth));
                m_PolygonLinesIndexes.push_back((int)m_PolygonLinesIndexes.size());
                m_PolygonLinesSourcePoint = _Target;
            }

            void arc_to(const gs_vec2f& _Target, const float& _Radius)
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

            void build_mesh(
                const RenderingQueueColor&         _Color,
                const RenderingQueueTexture&       _Texture,
                const float&                       _Width,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes)
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
                        m_PolygonLines[0].P1max,
                        m_PolygonLines[0].P2min,
                        m_PolygonLines[0].P2max,
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
                        m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].P1max,
                        m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].P2min,
                        m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].P2max,
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

            void build_mesh_filled_no_convex(
                const RenderingQueueColor&         _Color,
                const RenderingQueueTexture&       _Texture,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes)
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

        protected:

            std::vector<Immediate2DRendererPathSegment> m_PolygonLines           {std::vector<Immediate2DRendererPathSegment>()};
            float                                       m_PolygonLinesWidth      {4.f};
            std::vector<int>                            m_PolygonLinesIndexes    {std::vector<int>()};
            gs_vec2f                                    m_PolygonLinesSourcePoint{gs_vec2f(0.f, 0.f)};

            // service methods
            void end()
            {
                m_PolygonLines.clear();
                m_PolygonLinesIndexes.clear();
            }

            void build_triangle_filled_mesh(
                const gs_vec2f&                    _P1,
                const gs_vec2f&                    _P2,
                const gs_vec2f&                    _P3,
                const RenderingQueueColor&         _Color,
                const RenderingQueueTexture&       _Texture,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes)
            {
                const int size = (int)_Vertexes.size();

                _Vertexes.push_back(
                    RenderingQueueGraphicsApi::construct_vertex(
                        gs_vec3f(_P1.x, _P1.y, 0.f),
                        gs_vec3f(0.f), gs_vec2f(_P1.x / _Texture.Width, _P1.y / _Texture.Height),
                        _Color));
                
                _Vertexes.push_back(
                    RenderingQueueGraphicsApi::construct_vertex(
                        gs_vec3f(_P2.x, _P2.y, 0.f),
                        gs_vec3f(0.f), gs_vec2f(_P2.x / _Texture.Width, _P2.y / _Texture.Height),
                        _Color));
                
                _Vertexes.push_back(
                    RenderingQueueGraphicsApi::construct_vertex(
                        gs_vec3f(_P3.x, _P3.y, 0.f),
                        gs_vec3f(0.f),
                        gs_vec2f(_P3.x / _Texture.Width, _P3.y / _Texture.Height),
                        _Color));
                
                for (int i = size; i < (int)_Vertexes.size(); ++i)
                    _Indexes.push_back(i);
            }
        };

        class Immediate2DRenderer : public Layer
        {
        public:
            Immediate2DRenderer();
            virtual ~Immediate2DRenderer();

            // getters
            gs_vec3f              get_cursor_postion() const;
            RenderingQueueMetrics get_rendering_queue_metrics() const;
            float                 get_near_plane() const;
            float                 get_far_plane() const;

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_finish() override;
            virtual void finish() override;

            // API
            void push_rendering_command(
                const RenderingQueueTexture&            _Texture,
                const RenderingQueueColor&              _Color,
                const gs_mat4f&                         _Transform,
                const RenderingQueueGraphicsApiRenderingHints& _MeshRenderingHints = RenderingQueueGraphicsApiRenderingHints_::RenderingQueueGraphicsApiRenderingHints_Default);

            void push_rendering_command(
                const gs_mat4f&                         _Transform,
                const RenderingQueueGraphicsApiRenderingHints& _MeshRenderingHints = RenderingQueueGraphicsApiRenderingHints_::RenderingQueueGraphicsApiRenderingHints_Default);

            void push_clip_box(const gs_2dboxf& _Value, const gs_mat4f& _Transform = gs_mat4f(1.f));
            void pop_clip_box();

            void push_clear_color(const RenderingQueueColor& _Value);
            void pop_clear_color();
            
            gs_2dboxf           current_viewport() const;
            gs_2dboxf           current_clipping_box() const;
            RenderingQueueColor current_clear_color() const;

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

            gs_2dboxf calculate_bounding_box(
                const std::u32string&     _Text,
                const float&              _Size,
                const RenderingQueueFont& _Font);

            gs_2dboxf calculate_bounding_box(
                const std::u16string&     _Text,
                const float&              _Size,
                const RenderingQueueFont& _Font);

            gs_2dboxf calculate_bounding_box(
                const std::string&        _Text,
                const float&              _Size,
                const RenderingQueueFont& _Font);

            void push_triangle_filled(
                const gs_vec2f&              _P1,
                const gs_vec2f&              _P2,
                const gs_vec2f&              _P3,
                const RenderingQueueColor&   _Color,
                const gs_mat4f&              _Transform = gs_mat4f(1.f),
                const RenderingQueueTexture& _Texture   = RenderingQueueTexture());

            void push_rectangle_filled(
                const gs_vec2f&              _Min,
                const gs_vec2f&              _Max,
                const RenderingQueueColor&   _Color,
                const gs_mat4f&              _Transform = gs_mat4f(1.f),
                const RenderingQueueTexture& _Texture   = RenderingQueueTexture());

            void push_rectangle_gradient_mesh(
                const gs_vec2f&            _Min,
                const gs_vec2f&            _Max,
                const RenderingQueueColor& _Color1,
                const RenderingQueueColor& _Color2,
                const RenderingQueueColor& _Color3,
                const RenderingQueueColor& _Color4,
                const gs_mat4f&            _Transform = gs_mat4f(1.f));

            void push_rectangle_rounded_filled(
                const gs_vec2f&            _Min,
                const gs_vec2f&            _Max,
                const float&               _Radius,
                const RenderingQueueColor& _Color,
                const gs_mat4f&            _Transform              = gs_mat4f(1.f),
                bool                       _RoundTopLeftCorner     = true,
                bool                       _RoundTopRightCorner    = true,
                bool                       _RoundBottomRightCorner = true,
                bool                       _RoundBottomLeftCorner  = true);

            template<typename Type>
            void push_text(
                const Type&                _Begin,
                const Type&                _End,
                const float&               _Size,
                const RenderingQueueColor& _Color,
                const gs_mat4f&            _Transform = gs_mat4f(1.f),
                const RenderingQueueFont&  _Font      = RenderingQueueFont())
            {
                RenderingQueueFont font = _Font.is_null() ? m_RenderingQueue->get_default_font() : _Font;

                float scale     = _Size / (float)font.SizeInPixels;
                float offset    = (font.Ascent + font.Descent + font.LineGap) * scale;
                float positionX = 0.f;
                float positionY = gs_vec2f(0.f, offset).y;

                for(auto it = _Begin; it != _End; it++)
                {
                    unsigned int symbol = *it;

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

            void push_text(
                const std::u32string&      _Text,
                const float&               _Size,
                const RenderingQueueColor& _Color,
                const gs_mat4f&            _Transform = gs_mat4f(1.f),
                const RenderingQueueFont&  _Font      = RenderingQueueFont());

            void push_text(
                const std::u16string&      _Text,
                const float&               _Size,
                const RenderingQueueColor& _Color,
                const gs_mat4f&            _Transform = gs_mat4f(1.f),
                const RenderingQueueFont&  _Font      = RenderingQueueFont());

            void push_text(
                const std::string&         _Text,
                const float&               _Size,
                const RenderingQueueColor& _Color,
                const gs_mat4f&            _Transform = gs_mat4f(1.f),
                const RenderingQueueFont&  _Font      = RenderingQueueFont());

            void push_arc_filled(
                const gs_vec2f&              _Center,
                const float&                 _MinorRadius,
                const float&                 _MajorRadius,
                const float&                 _SourceAngle,
                const float&                 _TargetAngle,
                const RenderingQueueColor&   _Color,
                const gs_mat4f&              _Transform = gs_mat4f(1.f),
                const RenderingQueueTexture& _Texture   = RenderingQueueTexture());

            void push_line(
                const gs_vec2f&            _P1,
                const gs_vec2f&            _P2,
                const float&               _Width,
                const RenderingQueueColor& _Color,
                const gs_mat4f&            _Transform = gs_mat4f(1.f));

            void push_arc(
                const gs_vec2f&            _Center,
                const float&               _MinorRadius,
                const float&               _MajorRadius,
                const float&               _SourceAngle,
                const float&               _TargetAngle,
                const float&               _Width,
                const RenderingQueueColor& _Color,
                const gs_mat4f&            _Transform = gs_mat4f(1.f));

            void push_triangle(
                const gs_vec2f&            _P1,
                const gs_vec2f&            _P2,
                const gs_vec2f&            _P3,
                const float&               _Width,
                const RenderingQueueColor& _Color,
                const gs_mat4f&            _Transform = gs_mat4f(1.f));

            void push_rectangle(
                const gs_vec2f&            _Min,
                const gs_vec2f&            _Max,
                const float&               _Width,
                const RenderingQueueColor& _Color,
                const gs_mat4f&            _Transform = gs_mat4f(1.f));

            void push_rectangle_rounded(
                const gs_vec2f&            _Min,
                const gs_vec2f&            _Max,
                const float&               _Radius,
                const float&               _Width,
                const RenderingQueueColor& _Color,
                const gs_mat4f&            _Transform = gs_mat4f(1.f));

        private:

            // auxiliary mesh building API
            void build_triangle_filled_mesh(
                const gs_vec2f&              _P1,
                const gs_vec2f&              _P2,
                const gs_vec2f&              _P3,
                const RenderingQueueColor&   _Color,
                const RenderingQueueTexture& _Texture);

            void build_rectangle_filled_mesh(
                const gs_vec2f&              _Min,
                const gs_vec2f&              _Max,
                const RenderingQueueColor&   _Color,
                const RenderingQueueTexture& _Texture);

            void build_rectangle_filled_mesh(
                const gs_vec2f&            _Min,
                const gs_vec2f&            _Max,
                const gs_vec2f&            _MinUV,
                const gs_vec2f&            _MaxUV,
                const RenderingQueueColor& _Color);

            void build_rectangle_gradient_mesh(
                const gs_vec2f&            _Min,
                const gs_vec2f&            _Max,
                const RenderingQueueColor& _Color1,
                const RenderingQueueColor& _Color2,
                const RenderingQueueColor& _Color3,
                const RenderingQueueColor& _Color4);

            void build_arc_filled_mesh(
                const gs_vec2f&              _Center,
                const float&                 _MinorRadius,
                const float&                 _MajorRadius,
                const float&                 _SourceAngle,
                const float&                 _TargetAngle,
                const RenderingQueueColor&   _Color,
                const RenderingQueueTexture& _Texture,
                const int&                   _SegmentsCount = 36);

            void build_line_mesh(
                const gs_vec2f&              _P1,
                const gs_vec2f&              _P2,
                const float&                 _Width,
                const RenderingQueueColor&   _Color,
                const RenderingQueueTexture& _Texture);

            void build_arc_mesh(
                const gs_vec2f&            _Center,
                const float&               _MinorRadius,
                const float&               _MajorRadius,
                const float&               _SourceAngle,
                const float&               _TargetAngle,
                const float&               _Width,
                const RenderingQueueColor& _Color);

            // rendering queue data
            std::shared_ptr<RenderingQueue>   m_RenderingQueue                   {nullptr};
            gs_2dboxf                         m_RenderingQueueViewport           {gs_vec2f(-gs_huge<float>(), -gs_huge<float>()), gs_vec2f(+gs_huge<float>(), +gs_huge<float>())};
            std::vector<RenderingQueueColor>  m_RenderingQueueClearColors        {std::vector<RenderingQueueColor>()};
            std::vector<gs_2dboxf>            m_RenderingQueueClippingBoxes      {std::vector<gs_2dboxf>()};
            std::vector<RenderingQueueVertex> m_RenderingQueueMeshVertexes       {std::vector<RenderingQueueVertex>()};
            std::vector<int>                  m_RenderingQueueMeshVertexesIndexes{std::vector<int>()};
            float                             m_RenderingQueueMinimumLineWidth   {4.f};

            // path building data
            Immediate2DRendererPathBuilder    m_PathBuilder                      {Immediate2DRendererPathBuilder(8.f)};
        };
    }
}