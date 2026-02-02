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

            Immediate2DRendererPathBuilder(const gs_vec2f& _Point = gs_vec2f(0.f, 0.f)) : m_Source(_Point){}

            // TODO: add Bezier and random power curves here e.t.c
            void line_to(const gs_vec2f& _Target, std::vector<Immediate2DRendererPathSegment>& _Segments)
            {
                _Segments.push_back(Immediate2DRendererPathSegment(m_Source, _Target, m_DefaultWidth));
                m_Source = _Target;
            }

            void arc_to(const gs_vec2f& _Target, const float& _Radius, std::vector<Immediate2DRendererPathSegment>& _Segments)
            {
                // calculate ellipse data
                gs_complex<float> perpendicular  = gs_cnormf<float>(gs_complex<float>((_Target - m_Source).x, (_Target - m_Source).y)) * gs_complex<float>(0.f, 1.f);
                gs_vec2f          center         = (_Target + m_Source) * 0.5f + gs_vec2f(gs_realf(perpendicular), gs_imagf(perpendicular)) * _Radius;
                float             sourceAngle    = gs_to_degrees(atan2((m_Source - center).y, (m_Source - center).x));
                float             targetAngle    = gs_to_degrees(atan2((_Target - center).y, (_Target - center).x));
                float             radius         = (float)gs_vector_length(center - _Target);
                int               segmentsCount  = 36; // TODO: how to compute approximate ellipse segments count ???
                
                if(gs_abs(targetAngle - sourceAngle) < 1e-3) return;
                
                float angleIncrement = gs_abs((targetAngle - sourceAngle) / segmentsCount);

                for (float angle = sourceAngle; angle <= targetAngle; angle += angleIncrement)
                {
                    line_to(gs_vec2f(
                        center.x + radius * cos(gs_to_radians(angle)),
                        center.y + radius * sin(gs_to_radians(angle))),
                        _Segments);
                }

                line_to(_Target, _Segments);
            }

            void build_mesh(
                const RenderingQueueColor&                   _Color,
                const RenderingQueueTexture&                 _Texture,
                const float&                                 _Width,
                std::vector<Immediate2DRendererPathSegment>& _PolygonLines,
                std::vector<RenderingQueueVertex>&           _Vertexes,
                std::vector<int>&                            _Indexes)
            {
                if(_PolygonLines.empty())
                    return;

                if (_PolygonLines.size() == 1)
                {
                    _PolygonLines[0].setup(_Width);

                    build_triangle_filled_mesh(
                        _PolygonLines[0].P1min,
                        _PolygonLines[0].P2min,
                        _PolygonLines[0].P1max,
                        _Color,
                        _Texture,
                        _Vertexes,
                        _Indexes);

                    build_triangle_filled_mesh(
                        _PolygonLines[0].P1max,
                        _PolygonLines[0].P2min,
                        _PolygonLines[0].P2max,
                        _Color,
                        _Texture,
                        _Vertexes,
                        _Indexes);
                    
                    _PolygonLines.clear();

                    return;
                }
                
                auto get_element = [](const int& _Index, const int& _Size)->int
                {
                    int index = _Index;
                    while (index < 0)      index += _Size;
                    while (index >= _Size) index -= _Size;
                    return index;
                };

                const bool pathIsClosed = gs_vector_length(_PolygonLines[0].P1 - _PolygonLines[_PolygonLines.size() - 1].P2) < gs_epsilon<float>();

                for (int i = 0; i < (int)_PolygonLines.size(); i++)
                {
                    _PolygonLines[get_element(i, (int)_PolygonLines.size())].setup(_Width);

                    build_triangle_filled_mesh(
                        _PolygonLines[get_element(i, (int)_PolygonLines.size())].P1min,
                        _PolygonLines[get_element(i, (int)_PolygonLines.size())].P2min,
                        _PolygonLines[get_element(i, (int)_PolygonLines.size())].P1max,
                        _Color,
                        _Texture,
                        _Vertexes,
                        _Indexes);

                    build_triangle_filled_mesh(
                        _PolygonLines[get_element(i, (int)_PolygonLines.size())].P1max,
                        _PolygonLines[get_element(i, (int)_PolygonLines.size())].P2min,
                        _PolygonLines[get_element(i, (int)_PolygonLines.size())].P2max,
                        _Color,
                        _Texture,
                        _Vertexes,
                        _Indexes);

                    // TODO: need another algorithm of lines smoothing
                    if(i-1 >= 0 || pathIsClosed)
                    {
                        _PolygonLines[get_element(i-1, (int)_PolygonLines.size())].setup(_Width);

                        build_triangle_filled_mesh(
                            _PolygonLines[i].P1max,
                            _PolygonLines[get_element(i-1, (int)_PolygonLines.size())].P2max,
                            _PolygonLines[get_element(i-1, (int)_PolygonLines.size())].P2,
                            _Color,
                            _Texture,
                            _Vertexes,
                            _Indexes);

                        build_triangle_filled_mesh(
                            _PolygonLines[i].P1min,
                            _PolygonLines[get_element(i-1, (int)_PolygonLines.size())].P2min,
                            _PolygonLines[get_element(i-1, (int)_PolygonLines.size())].P2,
                            _Color,
                            _Texture,
                            _Vertexes,
                            _Indexes);
                    }
                }
                
                _PolygonLines.clear();
            }

            void build_mesh_filled_no_convex(
                const RenderingQueueColor&                   _Color,
                const RenderingQueueTexture&                 _Texture,
                std::vector<Immediate2DRendererPathSegment>& _PolygonLines,
                std::vector<int>&                            _PolygonTriangulatedIndexes,
                std::vector<RenderingQueueVertex>&           _Vertexes,
                std::vector<int>&                            _Indexes)
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
                if((int)_PolygonLines.size() < 3)
                {
                    _PolygonLines.clear();
                    _PolygonTriangulatedIndexes.clear();
                    return;
                }

                // triangle
                if((int)_PolygonLines.size() == 3)
                {
                    build_triangle_filled_mesh(
                        _PolygonLines[0].P1,
                        _PolygonLines[1].P1,
                        _PolygonLines[2].P1,
                        _Color,
                        _Texture,
                        _Vertexes,
                        _Indexes);

                    _PolygonLines.clear();
                    _PolygonTriangulatedIndexes.clear();
                    return;
                }

                // fill triangulation queue
                for (int i = 0; i < (int)_PolygonLines.size(); i++)
                {
                    _PolygonLines[i].Index = i;
                    _PolygonTriangulatedIndexes.push_back(i);
                }

                // triangulate polygon
                while (_PolygonTriangulatedIndexes.size() > 2)
                {
                    for (int i = 0; i < (int)_PolygonTriangulatedIndexes.size(); i++)
                    {
                        // construct triangle
                        int      ia = _PolygonTriangulatedIndexes[get_element(i, (int)_PolygonTriangulatedIndexes.size())    ];
                        int      ib = _PolygonTriangulatedIndexes[get_element(i - 1, (int)_PolygonTriangulatedIndexes.size())];
                        int      ic = _PolygonTriangulatedIndexes[get_element(i + 1, (int)_PolygonTriangulatedIndexes.size())];
                        gs_vec2f pa = _PolygonLines[ia].P1;
                        gs_vec2f pb = _PolygonLines[ib].P1;
                        gs_vec2f pc = _PolygonLines[ic].P1;

                        // detect if this triangle is an ear, i.e there are no other points besides
                        // this triangle points that are inside this triangle
                        bool isEar = true;

                        for (int j = 0; j < (int)_PolygonLines.size(); j++)
                        {
                            if( _PolygonLines[j].Index == _PolygonLines[ia].Index ||
                                _PolygonLines[j].Index == _PolygonLines[ib].Index ||
                                _PolygonLines[j].Index == _PolygonLines[ic].Index) continue;

                            gs_vec2f poly[3] = {pa, pb, pc};

                            if(gs_point_in_2D_polygon(poly, 3, gs_vec2f(_PolygonLines[j].P1)))
                            {
                                isEar = false;
                                break;
                            }
                        }

                        if(isEar || _PolygonTriangulatedIndexes.size() <= 3)
                        {
                            build_triangle_filled_mesh(pb, pa, pc, _Color, _Texture, _Vertexes, _Indexes);
                            _PolygonTriangulatedIndexes.erase(_PolygonTriangulatedIndexes.begin() + i);
                            break;
                        }
                    }
                }

                _PolygonLines.clear();
                _PolygonTriangulatedIndexes.clear();
            }

        protected:

            gs_vec2f m_Source       = gs_vec2f(0.f, 0.f);
            float    m_DefaultWidth = 4.f;

            // service methods
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
                    RenderingQueue::construct_vertex(
                        gs_vec3f(_P1.x, _P1.y, 0.f),
                        gs_vec3f(0.f), gs_vec2f(_P1.x / _Texture.Width, _P1.y / _Texture.Height),
                        _Color));
                
                _Vertexes.push_back(
                    RenderingQueue::construct_vertex(
                        gs_vec3f(_P2.x, _P2.y, 0.f),
                        gs_vec3f(0.f), gs_vec2f(_P2.x / _Texture.Width, _P2.y / _Texture.Height),
                        _Color));
                
                _Vertexes.push_back(
                    RenderingQueue::construct_vertex(
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
                const RenderingQueueMeshRenderingHints& _MeshRenderingHints = RenderingQueueMeshRenderingHints_::RenderingQueueMeshRenderingHints_Default);

            void push_rendering_command(
                const gs_mat4f&                         _Transform,
                const RenderingQueueMeshRenderingHints& _MeshRenderingHints = RenderingQueueMeshRenderingHints_::RenderingQueueMeshRenderingHints_Default);

            void push_clip_box(const gs_2dboxf& _Value, const gs_mat4f& _Transform = gs_mat4f(1.f));
            void pop_clip_box();

            void push_clear_color(const gs_vec4f& _Value);
            void pop_clear_color();
            
            gs_2dboxf current_clipping_box() const;
            gs_vec4f  current_clear_color() const;

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

            // path building and rendering API
            void begin_path(const gs_vec2f& _Source);

            void current_path_line_to(const gs_vec2f& _Target);
            void current_path_arc_to(const gs_vec2f& _Target, const float& _Radius);

            void push_current_path(const RenderingQueueColor& _Color, const float& _Width, const gs_mat4f& _Transform = gs_mat4f(1.f));
            void push_current_path_filled(const RenderingQueueColor& _Color, const gs_mat4f& _Transform = gs_mat4f(1.f));

            // graphical primitives rendering API
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

            void push_rectangle_rounded_filled(
                const gs_vec2f&            _Min,
                const gs_vec2f&            _Max,
                const float&               _Radius,
                const RenderingQueueColor& _Color,
                const gs_mat4f&            _Transform = gs_mat4f(1.f));

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
                const gs_vec2f&              _Center,
                const float&                 _MinorRadius,
                const float&                 _MajorRadius,
                const float&                 _SourceAngle,
                const float&                 _TargetAngle,
                const float&                 _LineWidth,
                const RenderingQueueColor&   _Color,
                const RenderingQueueTexture& _Texture,
                const int&                   _SegmentsCount = 36);

            // rendering queue data
            std::shared_ptr<RenderingQueue>             m_RenderingQueue        {nullptr};
            gs_2dboxf                                   m_RenderingQueueViewport{gs_vec2f(-gs_huge<float>(), -gs_huge<float>()), gs_vec2f(+gs_huge<float>(), +gs_huge<float>())};
            std::vector<gs_vec4f>                       m_RenderingQueueClearColors;
            std::vector<gs_2dboxf>                      m_RenderingQueueClippingBoxes;
            std::vector<RenderingQueueVertex>           m_RenderingQueueMeshVertexes;
            std::vector<int>                            m_RenderingQueueMeshVertexesIndexes;

            // path building data
            Immediate2DRendererPathBuilder              m_PathBuilder;
            std::vector<Immediate2DRendererPathSegment> m_PathBuilderPolygonLines;
            std::vector<int>                            m_PathBuilderPolygonLinesIndexes;
        };
    }
}