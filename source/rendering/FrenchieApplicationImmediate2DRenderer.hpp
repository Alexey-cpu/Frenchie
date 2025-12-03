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
        class Immediate2DRenderer : public Layer
        {
        public:
            Immediate2DRenderer();
            virtual ~Immediate2DRenderer();

            // getters
            std::shared_ptr<RenderingQueue> get_rendering_queue() const;

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_finish() override;
            virtual void finish() override;

            // API
            void push_rendering_command(
                const RenderingQueueTexture& _Texture,
                const gs_vec4f&              _Color,
                const gs_mat4f&              _Transform);

            gs_mat4f calculate_transform_matrix(
                const float&    _Depth,
                const gs_vec2f& _Position,
                const float&    _Rotation,
                const gs_vec2f& _Scale);

            gs_vec2f calculate_arc_point(
                const gs_vec2f& _Center,
                const float&    _MinorRadius,
                const float&    _MajorRadius,
                const float&    _ArcAngle);

            int calculate_arc_segments_number(float _Radius, float _MaximumError)
            {
                return (int)gs_clamp(gs_round_to_even((int)ceil(PI0 / acos(1 - gs_min((_MaximumError), (_Radius)) / (_Radius)))), 8, 512);
            }

            template<typename ...Args> 
            gs_2dboxf calculate_bounding_box(
                const float&    _Depth,
                const gs_vec2f& _Position,
                const float&    _Rotation,
                const gs_vec2f& _Scale,
                Args ...        _Args)
            {
                gs_mat4f _Transform =
                    calculate_transform_matrix(_Depth, _Position, _Rotation, _Scale);

                return gs_2dboxf(
                            _Transform * gs_vec4f(gs_vec2f(gs_min(static_cast<gs_vec2f>(_Args).x...), gs_min(static_cast<gs_vec2f>(_Args).y...)), 1.f, 1.f),
                            _Transform * gs_vec4f(gs_vec2f(gs_max(static_cast<gs_vec2f>(_Args).x...), gs_max(static_cast<gs_vec2f>(_Args).y...)), 1.f, 1.f));
            }

            gs_2dboxf calculate_bounding_box(
                const float&              _Depth,
                const gs_vec2f&           _Position,
                const float&              _Rotation,
                const gs_vec2f&           _Scale,
                const std::u32string&     _Text,
                const float&              _Size,
                const RenderingQueueFont& _Font);

            gs_2dboxf calculate_bounding_box(
                const float&              _Depth,
                const gs_vec2f&           _Position,
                const float&              _Rotation,
                const gs_vec2f&           _Scale,
                const std::u16string&     _Text,
                const float&              _Size,
                const RenderingQueueFont& _Font);

            gs_2dboxf calculate_bounding_box(
                const float&              _Depth,
                const gs_vec2f&           _Position,
                const float&              _Rotation,
                const gs_vec2f&           _Scale,
                const std::string&        _Text,
                const float&              _Size,
                const RenderingQueueFont& _Font);

            void push_triangle_filled(
                const gs_vec2f&              _P1,
                const gs_vec2f&              _P2,
                const gs_vec2f&              _P3,
                const gs_vec4f&              _Color,
                const float&                 _Depth,
                const gs_vec2f&              _Position = gs_vec2f(0.f, 0.f),
                const float&                 _Rotation = 0.f,
                const gs_vec2f&              _Scale    = gs_vec2f(1.f, 1.f),
                const RenderingQueueTexture& _Texture  = RenderingQueueTexture());

            void push_rectangle_filled(
                const gs_vec2f&              _Min,
                const gs_vec2f&              _Max,
                const gs_vec4f&              _Color,
                const float&                 _Depth,
                const gs_vec2f&              _Position = gs_vec2f(0.f, 0.f),
                const float&                 _Rotation = 0.f,
                const gs_vec2f&              _Scale    = gs_vec2f(1.f, 1.f),
                const RenderingQueueTexture& _Texture  = RenderingQueueTexture());

            void push_rectangle_rounded_filled(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Radius,
                const gs_vec4f& _Color,
                const float&    _Depth,
                const gs_vec2f& _Position = gs_vec2f(0.f, 0.f),
                const float&    _Rotation = 0.f,
                const gs_vec2f& _Scale    = gs_vec2f(1.f, 1.f));

            void push_text(
                const std::u32string&     _Text,
                const float&              _Size,
                const gs_vec4f&           _Color,
                const float&              _Depth,
                const gs_vec2f&           _Position = gs_vec2f(0.f, 0.f),
                const float&              _Rotation = 0.f,
                const gs_vec2f&           _Scale    = gs_vec2f(1.f, 1.f),
                const RenderingQueueFont& _Font     = RenderingQueueFont());

            void push_text(
                const std::u16string&     _Text,
                const float&              _Size,
                const gs_vec4f&           _Color,
                const float&              _Depth,
                const gs_vec2f&           _Position = gs_vec2f(0.f, 0.f),
                const float&              _Rotation = 0.f,
                const gs_vec2f&           _Scale    = gs_vec2f(1.f, 1.f),
                const RenderingQueueFont& _Font     = RenderingQueueFont());

            void push_text(
                const std::string&        _Text,
                const float&              _Size,
                const gs_vec4f&           _Color,
                const float&              _Depth,
                const gs_vec2f&           _Position = gs_vec2f(0.f, 0.f),
                const float&              _Rotation = 0.f,
                const gs_vec2f&           _Scale    = gs_vec2f(1.f, 1.f),
                const RenderingQueueFont& _Font     = RenderingQueueFont());

            void push_arc_filled(
                const gs_vec2f&              _Center,
                const float&                 _MinorRadius,
                const float&                 _MajorRadius,
                const float&                 _SourceAngle,
                const float&                 _TargetAngle,
                const gs_vec4f&              _Color,
                const float&                 _Depth,
                const gs_vec2f&              _Position = gs_vec2f(0.f, 0.f),
                const float&                 _Rotation = 0.f,
                const gs_vec2f&              _Scale    = gs_vec2f(1.f, 1.f),
                const RenderingQueueTexture& _Texture  = RenderingQueueTexture());

            void push_line(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const float&    _Width,
                const gs_vec4f& _Color,
                const float&    _Depth,
                const gs_vec2f& _Position = gs_vec2f(0.f, 0.f),
                const float&    _Rotation = 0.f,
                const gs_vec2f& _Scale    = gs_vec2f(1.f, 1.f));

            void push_arc(
                const gs_vec2f& _Center,
                const float&    _MinorRadius,
                const float&    _MajorRadius,
                const float&    _SourceAngle,
                const float&    _TargetAngle,
                const float&    _Width,
                const gs_vec4f& _Color,
                const float&    _Depth,
                const gs_vec2f& _Position = gs_vec2f(0.f, 0.f),
                const float&    _Rotation = 0.f,
                const gs_vec2f& _Scale    = gs_vec2f(1.f, 1.f));

            void push_triangle(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const gs_vec2f& _P3,
                const float&    _Width,
                const gs_vec4f& _Color,
                const float&    _Depth,
                const gs_vec2f& _Position = gs_vec2f(0.f, 0.f),
                const float&    _Rotation = 0.f,
                const gs_vec2f& _Scale    = gs_vec2f(1.f, 1.f));

            void push_rectangle(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Width,
                const gs_vec4f& _Color,
                const float&    _Depth,
                const gs_vec2f& _Position = gs_vec2f(0.f, 0.f),
                const float&    _Rotation = 0.f,
                const gs_vec2f& _Scale    = gs_vec2f(1.f, 1.f));

            void push_rectangle_rounded(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Radius,
                const float&    _Width,
                const gs_vec4f& _Color,
                const float&    _Depth,
                const gs_vec2f& _Position = gs_vec2f(0.f, 0.f),
                const float&    _Rotation = 0.f,
                const gs_vec2f& _Scale    = gs_vec2f(1.f, 1.f));

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
                const gs_vec2f&                    _Center,
                const float&                       _MinorRadius,
                const float&                       _MajorRadius,
                const float&                       _SourceAngle,
                const float&                       _TargetAngle,
                const float&                       _LineWidth,
                const gs_vec4f&                    _Color,
                const RenderingQueueTexture&       _Texture,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            // this is a plipeline
            std::vector<RenderingQueueVertex> m_Vertexes      {std::vector<RenderingQueueVertex>()};
            std::vector<int>                  m_Indexes       {std::vector<int>()};
            gs_2dboxf                         m_Viewport      {gs_vec2f(-gs_huge<float>(), -gs_huge<float>()), gs_vec2f(+gs_huge<float>(), +gs_huge<float>())};
            std::shared_ptr<RenderingQueue>   m_RenderingQueue{nullptr};
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