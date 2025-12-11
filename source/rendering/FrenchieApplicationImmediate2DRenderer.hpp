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
        class Immediate2DRendererPathSegment
        {
        public:
            Immediate2DRendererPathSegment(const gs_vec2f& _P1, const gs_vec2f& _P2, const float _Width = 4.f)
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

            gs_vec2f P1;
            gs_vec2f P1min;
            gs_vec2f P1max;

            gs_vec2f P2;
            gs_vec2f P2min;
            gs_vec2f P2max;
        };

        class Immediate2DRendererPathBuilder
        {
        public:

            void push_segment(
                const gs_vec2f&                              _P1,
                const gs_vec2f&                              _P2,
                const float                                  _Width,
                std::vector<Immediate2DRendererPathSegment>& _Segments)
            {
                _Segments.push_back(Immediate2DRendererPathSegment(_P1, _P2, _Width));
            }

            void build_mesh(
                const gs_vec4f&                    _Color,
                const RenderingQueueTexture&       _Texture,
                std::vector<Immediate2DRendererPathSegment>&          _Segments,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes)
            {
                if(_Segments.empty())
                    return;

                if (_Segments.size() == 1)
                {
                    build_triangle_filled_mesh(
                        _Segments[0].P1min,
                        _Segments[0].P2min,
                        _Segments[0].P1max,
                        _Color,
                        _Texture,
                        _Vertexes,
                        _Indexes);

                    build_triangle_filled_mesh(
                        _Segments[0].P1max,
                        _Segments[0].P2min,
                        _Segments[0].P2max,
                        _Color,
                        _Texture,
                        _Vertexes,
                        _Indexes);
                    
                    _Segments.clear();

                    return;
                }
                
                auto get_element = [](const int& _Index, const int& _Size)->int
                {
                    int index = _Index;
                    while (index < 0)      index += _Size;
                    while (index >= _Size) index -= _Size;
                    return index;
                };

                for (int i = 1; i < (int)_Segments.size(); i++)
                {
                    build_triangle_filled_mesh(
                        _Segments[i].P1min,
                        _Segments[i].P2min,
                        _Segments[i].P1max,
                        _Color,
                        _Texture,
                        _Vertexes,
                        _Indexes);

                    build_triangle_filled_mesh(
                        _Segments[i].P1max,
                        _Segments[i].P2min,
                        _Segments[i].P2max,
                        _Color,
                        _Texture,
                        _Vertexes,
                        _Indexes);

                    build_triangle_filled_mesh(
                        _Segments[i].P1max,
                        _Segments[get_element(i-1, (int)_Segments.size())].P2max,
                        _Segments[get_element(i-1, (int)_Segments.size())].P2,
                        _Color,
                        _Texture,
                        _Vertexes,
                        _Indexes);

                    build_triangle_filled_mesh(
                        _Segments[i].P1min,
                        _Segments[get_element(i-1, (int)_Segments.size())].P2min,
                        _Segments[get_element(i-1, (int)_Segments.size())].P2,
                        _Color,
                        _Texture,
                        _Vertexes,
                        _Indexes);
                }
                
                _Segments.clear();
            }

        protected:

            // service methods
            void build_triangle_filled_mesh(
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
        };

        class Immediate2DRenderer : public Layer
        {
        public:
            Immediate2DRenderer();
            virtual ~Immediate2DRenderer();

            // getters
            gs_vec3f get_cursor_postion() const
            {
                gs_vec2f size   = Frenchie::Application::application()->get_window_size();
                gs_vec2f cursor = Frenchie::Application::application()->get_window_cursor_position();
                gs_mat4f matrix =
                    gs_matrix_invert_square(m_RenderingQueue->get_cameraview_matrix()) *
                    gs_matrix_invert_square(m_RenderingQueue->get_projection_matrix());
                
                return matrix * gs_vec4f(gs_vector_convert_to_NDC(cursor, size), -1.f, 1.f);
            }

            float get_near_plane() const
            {
                return -1000.f;
            }

            float get_far_plane() const
            {
                return +1000.f;
            }

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_finish() override;
            virtual void finish() override;

            // API
            void push_rendering_command(
                const RenderingQueueTexture&            _Texture,
                const gs_vec4f&                         _Color,
                const gs_mat4f&                         _Transform,
                const RenderingQueueMeshRenderingHints& _MeshRenderingHints = RenderingQueueMeshRenderingHints_::RenderingQueueMeshRenderingHints_Default);

            void push_clip_box(const gs_2dboxf& _Value, const gs_mat4f& _Transform = gs_mat4f(1.f))
            {
                m_Clipbox.push_back(_Value.transform(_Transform));
                m_RenderingQueue->push_renderer_command(_Value.transform(_Transform));
            }

            void pop_clip_box()
            {
                if(!m_Clipbox.empty())m_Clipbox.pop_back();

                if(!m_Clipbox.empty())
                {
                    m_RenderingQueue->push_renderer_command(m_Clipbox[m_Clipbox.size() - 1]);
                }
                else
                {
                    m_RenderingQueue->push_renderer_command(gs_2dboxf(gs_vec2f(0.f, 0.f), application()->get_window_size()));                    
                }
            }

            void push_clear_color(const gs_vec4f& _Value)
            {
                m_RenderingQueue->push_renderer_command(_Value);
            }

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
                const gs_vec4f&              _Color,
                const gs_mat4f&              _Transform = gs_mat4f(1.f),
                const RenderingQueueTexture& _Texture   = RenderingQueueTexture());

            void push_rectangle_filled(
                const gs_vec2f&              _Min,
                const gs_vec2f&              _Max,
                const gs_vec4f&              _Color,
                const gs_mat4f&              _Transform = gs_mat4f(1.f),
                const RenderingQueueTexture& _Texture   = RenderingQueueTexture());

            void push_rectangle_rounded_filled(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Radius,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            void push_text(
                const std::u32string&     _Text,
                const float&              _Size,
                const gs_vec4f&           _Color,
                const gs_mat4f&           _Transform = gs_mat4f(1.f),
                const RenderingQueueFont& _Font      = RenderingQueueFont());

            void push_text(
                const std::u16string&     _Text,
                const float&              _Size,
                const gs_vec4f&           _Color,
                const gs_mat4f&           _Transform = gs_mat4f(1.f),
                const RenderingQueueFont& _Font      = RenderingQueueFont());

            void push_text(
                const std::string&        _Text,
                const float&              _Size,
                const gs_vec4f&           _Color,
                const gs_mat4f&           _Transform = gs_mat4f(1.f),
                const RenderingQueueFont& _Font      = RenderingQueueFont());

            void push_arc_filled(
                const gs_vec2f&              _Center,
                const float&                 _MinorRadius,
                const float&                 _MajorRadius,
                const float&                 _SourceAngle,
                const float&                 _TargetAngle,
                const gs_vec4f&              _Color,
                const gs_mat4f&              _Transform = gs_mat4f(1.f),
                const RenderingQueueTexture& _Texture   = RenderingQueueTexture());

            void push_line(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const float&    _Width,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            void push_arc(
                const gs_vec2f& _Center,
                const float&    _MinorRadius,
                const float&    _MajorRadius,
                const float&    _SourceAngle,
                const float&    _TargetAngle,
                const float&    _Width,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            void push_triangle(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const gs_vec2f& _P3,
                const float&    _Width,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            void push_rectangle(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Width,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            void push_rectangle_rounded(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Radius,
                const float&    _Width,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform = gs_mat4f(1.f));

            // static API
            static void build_triangle_filled_mesh(
                const gs_vec2f&                    _P1,
                const gs_vec2f&                    _P2,
                const gs_vec2f&                    _P3,
                const gs_vec4f&                    _Color,
                const RenderingQueueTexture&       _Texture,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            static void build_rectangle_filled_mesh(
                const gs_vec2f&                    _Min,
                const gs_vec2f&                    _Max,
                const gs_vec4f&                    _Color,
                const RenderingQueueTexture&       _Texture,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            static void build_rectangle_filled_mesh(
                const gs_vec2f&                    _Min,
                const gs_vec2f&                    _Max,
                const gs_vec2f&                    _MinUV,
                const gs_vec2f&                    _MaxUV,
                const gs_vec4f&                    _Color,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            static void build_arc_filled_mesh(
                const gs_vec2f&                    _Center,
                const float&                       _MinorRadius,
                const float&                       _MajorRadius,
                const float&                       _SourceAngle,
                const float&                       _TargetAngle,
                const gs_vec4f&                    _Color,
                const RenderingQueueTexture&       _Texture,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes,
                const int&                         _SegmentsCount = 36);

            static void build_line_mesh(
                const gs_vec2f&                    _P1,
                const gs_vec2f&                    _P2,
                const float&                       _Width,
                const gs_vec4f&                    _Color,
                const RenderingQueueTexture&       _Texture,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            static void build_arc_mesh(
                const gs_vec2f&                           _Center,
                const float&                              _MinorRadius,
                const float&                              _MajorRadius,
                const float&                              _SourceAngle,
                const float&                              _TargetAngle,
                const float&                              _LineWidth,
                const gs_vec4f&                           _Color,
                const RenderingQueueTexture&              _Texture,
                std::vector<Immediate2DRendererPathSegment>& _Segments,
                std::vector<RenderingQueueVertex>&        _Vertexes,
                std::vector<int>&                         _Indexes,
                const int&                                _SegmentsCount = 36);

            static gs_vec2f left(const gs_vec2f& _Vector)
            {
                return gs_vec2f(-1.f, 0.f) * _Vector;
            }

            static gs_vec2f right(const gs_vec2f& _Vector)
            {
                return gs_vec2f(+1.f, 0.f) * _Vector;
            }

            static gs_vec2f top(const gs_vec2f& _Vector)
            {
                return gs_vec2f(0.f, -1.f) * _Vector;
            }

            static gs_vec2f down(const gs_vec2f& _Vector)
            {
                return gs_vec2f(0.f, +1.f) * _Vector;
            }

            static gs_vec2f top_left(const gs_vec2f& _Vector)
            {
                return gs_vec2f(Immediate2DRenderer::left(_Vector).x, Immediate2DRenderer::top(_Vector).y);
            }

            static gs_vec2f top_right(const gs_vec2f& _Vector)
            {
                return gs_vec2f(Immediate2DRenderer::right(_Vector).x, Immediate2DRenderer::top(_Vector).y);
            }

            static gs_vec2f bottom_left(const gs_vec2f& _Vector)
            {
                return gs_vec2f(Immediate2DRenderer::left(_Vector).x, Immediate2DRenderer::down(_Vector).y);
            }

            static gs_vec2f bottom_right(const gs_vec2f& _Vector)
            {
                return gs_vec2f(Immediate2DRenderer::right(_Vector).x, Immediate2DRenderer::down(_Vector).y);
            }

            // this is a plipeline
            std::vector<gs_2dboxf>                      m_Clipbox       {std::vector<gs_2dboxf>()};
            std::vector<Immediate2DRendererPathSegment> m_Segmetns      {std::vector<Immediate2DRendererPathSegment>()};
            std::vector<RenderingQueueVertex>           m_Vertexes      {std::vector<RenderingQueueVertex>()};
            std::vector<int>                            m_Indexes       {std::vector<int>()};
            gs_2dboxf                                   m_Viewport      {gs_vec2f(-gs_huge<float>(), -gs_huge<float>()), gs_vec2f(+gs_huge<float>(), +gs_huge<float>())};
            std::shared_ptr<RenderingQueue>             m_RenderingQueue{nullptr};
        };
    }
}

// template<typename ...Args>
// void push_polygon(

//     const float&                       _Depth,
//     const RenderingQueueTexture&       _Texture,
//     const gs_vec4f&                    _Color,
//     const gs_mat4f&                    _Transform,
//     Args ...                           _Args);

// template<typename Head, typename ...Args>
// void push_polygon(
//     const float&                       _Depth,
//     const RenderingQueueTexture&       _Texture,
//     const gs_vec4f&                    _Color,
//     const gs_mat4f&                    _Transform,
//     const Head&                        _Point,
//     Args ...                           _Args)
// {
//     m_TriangulationQueue.push_back(
//         RenderingQueueVertex(
//             gs_vec3f(static_cast<gs_vec2f>(_Point), 1.f),
//             gs_vec3f(0.f),
//             gs_vec2f(0.f),
//             _Color
//         )
//     );

//     push_polygon(_Depth, _Texture, _Color, _Transform, static_cast<gs_vec2f>(_Args)...);
// }

// template<>
// void push_polygon(
//     const float&                       _Depth,
//     const RenderingQueueTexture&       _Texture,
//     const gs_vec4f&                    _Color,
//     const gs_mat4f&                    _Transform)
// {
//     auto get_item = [](
//         const std::vector<int>& _Vertexes,
//         const int&              _Index)->int
//     {
//         int index = _Index;

//         while(index < 0)
//             index += (int)_Vertexes.size();

//         while(index >= (int)_Vertexes.size())
//             index -= (int)_Vertexes.size();

//         return _Vertexes[index];
//     };

//     for (int i = 0; i < (int)m_TriangulationQueue.size(); i++)
//         m_TriangulationIndexes.push_back(i);

//     // triangulate
//     while (m_TriangulationIndexes.size() > 2)
//     {
//         for (int i = 0; i < (int)m_TriangulationIndexes.size(); i++)
//         {
//             int ia = get_item(m_TriangulationIndexes, i);
//             int ib = get_item(m_TriangulationIndexes, i - 1);
//             int ic = get_item(m_TriangulationIndexes, i + 1);

//             auto a = m_TriangulationQueue[ia].Position;
//             auto b = m_TriangulationQueue[ib].Position;
//             auto c = m_TriangulationQueue[ic].Position;

//             if(gs_vector_cross(gs_vec2f(b - a), gs_vec2f(c - a)) < 0)
//                 continue;

//             bool isEar = true;

//             for (int j = 0; j < (int)m_TriangulationQueue.size(); j++)
//             {
//                 if(j == ia || j == ib || j == ic)
//                     continue;

//                 gs_vec2f poly[3] = {a, b, c};

//                 if(gs_point_in_2D_polygon(poly, 3, gs_vec2f(m_TriangulationQueue[j].Position)))
//                 {
//                     isEar = false;
//                     break;
//                 }
//             }

//             if(isEar)
//             {
//                 m_Vertexes.push_back(b);
//                 m_Vertexes.push_back(a);
//                 m_Vertexes.push_back(c);
//                 m_TriangulationIndexes.erase(m_TriangulationIndexes.begin() + i);
//                 break;
//             }
//         }
//     }
    
//     for (int i = 0; i < m_Vertexes.size(); i++)
//         m_Indexes.push_back(i);

//     m_TriangulationQueue.clear();
//     m_TriangulationIndexes.clear();

//     push_rendering_command(m_DefaultTexture, _Color, _Transform);
// }