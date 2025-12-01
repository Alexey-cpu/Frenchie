#pragma once

// Application
#include <FrenchieApplication.hpp>

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

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_render() override;
            virtual void finish() override;

            // command API
            void push_rendering_command(
                const RenderingQueueTexture& _Texture,
                const gs_vec4f&              _Color,
                const gs_mat4f&              _Transform);

            // rendering API
            bool push_triangle_filled(
                const gs_vec2f&              _P1,
                const gs_vec2f&              _P2,
                const gs_vec2f&              _P3,
                const gs_vec4f&              _Color,
                const float&                 _Depth,
                const gs_vec2f&              _Position = gs_vec2f(0.f, 0.f),
                const float&                 _Rotation = 0.f,
                const gs_vec2f&              _Scale    = gs_vec2f(1.f, 1.f),
                const RenderingQueueTexture& _Texture  = RenderingQueueTexture());

            bool push_rectangle_filled(
                const gs_vec2f&              _Min,
                const gs_vec2f&              _Max,
                const gs_vec4f&              _Color,
                const float&                 _Depth,
                const gs_vec2f&              _Position = gs_vec2f(0.f, 0.f),
                const float&                 _Rotation = 0.f,
                const gs_vec2f&              _Scale    = gs_vec2f(1.f, 1.f),
                const RenderingQueueTexture& _Texture  = RenderingQueueTexture());

            bool push_rectangle_rounded_filled(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Radius,
                const gs_vec4f& _Color,
                const float&    _Depth,
                const gs_vec2f& _Position = gs_vec2f(0.f, 0.f),
                const float&    _Rotation = 0.f,
                const gs_vec2f& _Scale    = gs_vec2f(1.f, 1.f));

            bool push_utf32_text(
                const std::u32string&     _Text,
                const float&              _Size,
                const gs_vec4f&           _Color,
                const float&              _Depth,
                const gs_vec2f&           _Position = gs_vec2f(0.f, 0.f),
                const float&              _Rotation = 0.f,
                const gs_vec2f&           _Scale    = gs_vec2f(1.f, 1.f),
                const RenderingQueueFont& _Font     = RenderingQueueFont());

            bool push_utf16_text(
                const std::u16string&     _Text,
                const float&              _Size,
                const gs_vec4f&           _Color,
                const float&              _Depth,
                const gs_vec2f&           _Position = gs_vec2f(0.f, 0.f),
                const float&              _Rotation = 0.f,
                const gs_vec2f&           _Scale    = gs_vec2f(1.f, 1.f),
                const RenderingQueueFont& _Font     = RenderingQueueFont());

            bool push_utf8_text(
                const std::string&        _Text,
                const float&              _Size,
                const gs_vec4f&           _Color,
                const float&              _Depth,
                const gs_vec2f&           _Position = gs_vec2f(0.f, 0.f),
                const float&              _Rotation = 0.f,
                const gs_vec2f&           _Scale    = gs_vec2f(1.f, 1.f),
                const RenderingQueueFont& _Font     = RenderingQueueFont());

            bool push_arc_filled(
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

            bool push_line(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const float&    _Width,
                const gs_vec4f& _Color,
                const float&    _Depth,
                const gs_vec2f& _Position = gs_vec2f(0.f, 0.f),
                const float&    _Rotation = 0.f,
                const gs_vec2f& _Scale    = gs_vec2f(1.f, 1.f));

            bool push_arc(
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

            bool push_triangle(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const gs_vec2f& _P3,
                const float&    _Width,
                const gs_vec4f& _Color,
                const float&    _Depth,
                const gs_vec2f& _Position = gs_vec2f(0.f, 0.f),
                const float&    _Rotation = 0.f,
                const gs_vec2f& _Scale    = gs_vec2f(1.f, 1.f));

            bool push_rectangle(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Width,
                const gs_vec4f& _Color,
                const float&    _Depth,
                const gs_vec2f& _Position = gs_vec2f(0.f, 0.f),
                const float&    _Rotation = 0.f,
                const gs_vec2f& _Scale    = gs_vec2f(1.f, 1.f));

            bool push_rectangle_rounded(
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

            void build_arc_filled_mesh(
                const gs_vec2f&                    _Center,
                const float&                       _MinorRadius,
                const float&                       _MajorRadius,
                const float&                       _SourceAngle,
                const float&                       _TargetAngle,
                const gs_vec4f&                    _Color,
                const RenderingQueueTexture&       _Texture,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

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

            static gs_mat4f construct_transform_matrix(
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

            template<typename ...Args> 
            gs_rectf calculate_bounding_box(
                const float&    _Depth,
                const gs_vec2f& _Position,
                const float&    _Rotation,
                const gs_vec2f& _Scale,
                Args ...        _Args)
            {
                gs_mat4f _Transform =
                    construct_transform_matrix(_Depth, _Position, _Rotation, _Scale);

                return gs_rectf(
                            _Transform * gs_vec4f(gs_vec2f(gs_min(static_cast<gs_vec2f>(_Args).x...), gs_min(static_cast<gs_vec2f>(_Args).y...)), 1.f, 1.f),
                            _Transform * gs_vec4f(gs_vec2f(gs_max(static_cast<gs_vec2f>(_Args).x...), gs_max(static_cast<gs_vec2f>(_Args).y...)), 1.f, 1.f));
            }

            gs_rectf calculate_bounding_box(
                const float&              _Depth,
                const gs_vec2f&           _Position,
                const float&              _Rotation,
                const gs_vec2f&           _Scale,
                const std::u32string&     _Text,
                const float&              _Size,
                const RenderingQueueFont& _Font);

            gs_rectf calculate_bounding_box(
                const float&              _Depth,
                const gs_vec2f&           _Position,
                const float&              _Rotation,
                const gs_vec2f&           _Scale,
                const std::u16string&     _Text,
                const float&              _Size,
                const RenderingQueueFont& _Font);

            gs_rectf calculate_bounding_box(
                const float&              _Depth,
                const gs_vec2f&           _Position,
                const float&              _Rotation,
                const gs_vec2f&           _Scale,
                const std::string&        _Text,
                const float&              _Size,
                const RenderingQueueFont& _Font);

            // this is a plipeline
            std::vector<RenderingQueueVertex>  m_Vertexes      {std::vector<RenderingQueueVertex>()};
            std::vector<int>                   m_Indexes       {std::vector<int>()};
            gs_rectf                           m_Viewport      {-gs_huge<float>(), -gs_huge<float>(), +gs_huge<float>(), +gs_huge<float>()}; 
            RenderingQueueShader               m_DefaultShader {RenderingQueueShader()};
            RenderingQueueTexture              m_DefaultTexture{RenderingQueueTexture()};
            RenderingQueueFont                 m_DefaultFont   {RenderingQueueFont()};
        };

        class Immediate2DRendererDefaultFont
        {
        public:
            static unsigned int  COMPRESSED_SIZE;
            static unsigned char BUFFER[316235];
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