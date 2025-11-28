#pragma once

#include <FrenchieApplication.hpp>

#include <chrono>
#include <iostream>

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
            void push_triangle_filled(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const gs_vec2f& _P3,
                const float&    _Depth,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform,
                const RenderingQueueTexture& _Texture  = RenderingQueueTexture());

            void push_rectangle_filled(
                const gs_vec2f&              _Min,
                const gs_vec2f&              _Max,
                const float&                 _Depth,
                const gs_vec4f&              _Color,
                const gs_mat4f&              _Transform,
                const RenderingQueueTexture& _Texture  = RenderingQueueTexture());

            void push_arc_filled(
                const gs_vec2f&                _Center,
                const float&                   _MinorRadius,
                const float&                   _MajorRadius,
                const float&                   _SourceAngle,
                const float&                   _TargetAngle,
                const float&                   _Depth,
                const gs_vec4f&                _Color,
                const gs_mat4f&                _Transform,
                const RenderingQueueTexture&   _Texture = RenderingQueueTexture());

            void push_text(
                const std::string&        _Text,
                const float&              _Size,
                const gs_vec2f&           _Position,
                const gs_vec4f&           _Color,
                const gs_mat4f&           _Transform,
                const RenderingQueueFont& _Font = RenderingQueueFont());

            void push_line(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const float&    _Depth,
                const float&    _LineWidth,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform);

            void push_arc(
                const gs_vec2f& _Center,
                const float&    _MinorRadius,
                const float&    _MajorRadius,
                const float&    _SourceAngle,
                const float&    _TargetAngle,
                const float&    _Depth,
                const float&    _LineWidth,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform);

            void push_triangle(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const gs_vec2f& _P3,
                const float&    _Depth,
                const float&    _LineWidth,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform);

            void push_rectangle(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Depth,
                const float&    _LineWidth,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform);

            void push_rectangle_rounded(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Radius,
                const float&    _Depth,
                const float&    _LineWidth,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform);

            template<typename ...Args> 
            gs_rectf calculate_bounding_box(const gs_mat4f& _Transform, Args ... _Args)
            {
                return gs_rectf(
                            _Transform * gs_vec4f(gs_vec2f(gs_min(static_cast<gs_vec2f>(_Args).x...), gs_min(static_cast<gs_vec2f>(_Args).y...)), 1.f, 1.f),
                            _Transform * gs_vec4f(gs_vec2f(gs_max(static_cast<gs_vec2f>(_Args).x...), gs_max(static_cast<gs_vec2f>(_Args).y...)), 1.f, 1.f));
            }

            template<typename ...Args>
            void push_polygon(

                const float&                       _Depth,
                const RenderingQueueTexture&       _Texture,
                const gs_vec4f&                    _Color,
                const gs_mat4f&                    _Transform,
                Args ...                           _Args);

            template<typename Head, typename ...Args>
            void push_polygon(
                const float&                       _Depth,
                const RenderingQueueTexture&       _Texture,
                const gs_vec4f&                    _Color,
                const gs_mat4f&                    _Transform,
                const Head&                        _Point,
                Args ...                           _Args)
            {
                m_TriangulationQueue.push_back(
                    RenderingQueueVertex(
                        gs_vec3f(static_cast<gs_vec2f>(_Point), 1.f),
                        gs_vec3f(0.f),
                        gs_vec2f(0.f),
                        _Color
                    )
                );

                push_polygon(_Depth, _Texture, _Color, _Transform, static_cast<gs_vec2f>(_Args)...);
            }

            template<>
            void push_polygon(
                const float&                       _Depth,
                const RenderingQueueTexture&       _Texture,
                const gs_vec4f&                    _Color,
                const gs_mat4f&                    _Transform)
            {
                auto get_item = [](
                    const std::vector<int>& _Vertexes,
                    const int&              _Index)->int
                {
                    int index = _Index;

                    while(index < 0)
                        index += (int)_Vertexes.size();

                    while(index >= (int)_Vertexes.size())
                        index -= (int)_Vertexes.size();

                    return _Vertexes[index];
                };

                auto triangle_contains_point = [](
                    const gs_vec2f& _P,
                    const gs_vec2f& _A,
                    const gs_vec2f& _B,
                    const gs_vec2f& _C)
                {
                    return !(gs_vector_cross(gs_vec2f(_B - _A), gs_vec2f(_P - _A)) > 0.f) &&
                           !(gs_vector_cross(gs_vec2f(_C - _B), gs_vec2f(_P - _B)) > 0.f) &&
                           !(gs_vector_cross(gs_vec2f(_A - _C), gs_vec2f(_P - _C)) > 0.f);
                };

                for (int i = 0; i < (int)m_TriangulationQueue.size(); i++)
                    m_TriangulationIndexes.push_back(i);

                // triangulate
                while (m_TriangulationIndexes.size() > 2)
                {
                    for (int i = 0; i < (int)m_TriangulationIndexes.size(); i++)
                    {
                        int ia = get_item(m_TriangulationIndexes, i);
                        int ib = get_item(m_TriangulationIndexes, i - 1);
                        int ic = get_item(m_TriangulationIndexes, i + 1);

                        auto a = m_TriangulationQueue[ia].Position;
                        auto b = m_TriangulationQueue[ib].Position;
                        auto c = m_TriangulationQueue[ic].Position;

                        if(gs_vector_cross(gs_vec2f(b - a), gs_vec2f(c - a)) < 0)
                            continue;

                        bool isEar = true;

                        for (int j = 0; j < (int)m_TriangulationQueue.size(); j++)
                        {
                            if(j == ia || j == ib || j == ic)
                                continue;

                            if(triangle_contains_point(m_TriangulationQueue[j].Position, b, a, c))
                            {
                                isEar = false;
                                break;
                            }
                        }

                        if(isEar)
                        {
                            m_Vertexes.push_back(b);
                            m_Vertexes.push_back(a);
                            m_Vertexes.push_back(c);
                            m_TriangulationIndexes.erase(m_TriangulationIndexes.begin() + i);
                            break;
                        }
                    }
                }
                
                for (int i = 0; i < m_Vertexes.size(); i++)
                    m_Indexes.push_back(i);

                push_rendering_command(m_DefaultTexture, _Color, _Transform);
            }

            // static API
            static void build_triangle_filled_mesh(
                const gs_vec2f&                    _P1,
                const gs_vec2f&                    _P2,
                const gs_vec2f&                    _P3,
                const float&                       _Depth,
                const RenderingQueueTexture&       _Texture,
                const gs_vec4f&                    _Color,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            static void build_rectangle_filled_mesh(
                const gs_vec2f&                    _Min,
                const gs_vec2f&                    _Max,
                const float&                       _Depth,
                const RenderingQueueTexture&       _Texture,
                const gs_vec4f&                    _Color,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            static void build_rectangle_filled_mesh(
                const gs_vec2f&                    _Min,
                const gs_vec2f&                    _Max,
                const gs_vec2f&                    _MinUV,
                const gs_vec2f&                    _MaxUV,
                const gs_vec4f&                    _Color,
                const float&                       _Depth,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            void build_arc_filled_mesh(
                const gs_vec2f&                    _Center,
                const float&                       _MinorRadius,
                const float&                       _MajorRadius,
                const float&                       _SourceAngle,
                const float&                       _TargetAngle,
                const float&                       _Depth,
                const gs_vec4f&                    _Color,
                const gs_mat4f&                    _Transform,
                const RenderingQueueTexture&       _Texture,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            static void build_line_mesh(
                const gs_vec2f&                    _P1,
                const gs_vec2f&                    _P2,
                const float&                       _Depth,
                const float&                       _LineWidth,
                const RenderingQueueTexture&       _Texture,
                const gs_vec4f&                    _Color,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            static void build_arc_mesh(
                const gs_vec2f&                    _Center,
                const float&                       _MinorRadius,
                const float&                       _MajorRadius,
                const float&                       _SourceAngle,
                const float&                       _TargetAngle,
                const float&                       _Depth,
                const float&                       _LineWidth,
                const RenderingQueueTexture&       _Texture,
                const gs_vec4f&                    _Color,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            static gs_rectf calculate_text_bounding_box(
                const std::string&        _Text,
                const float&              _Size,
                const gs_vec2f&           _Position,
                const gs_mat4f&           _Transform,
                const RenderingQueueFont& _Font);

            // this is a plipeline
            std::vector<RenderingQueueVertex>  m_TriangulationQueue      {std::vector<RenderingQueueVertex>()};
            std::vector<int>                   m_TriangulationIndexes    {std::vector<int>()};
            
            std::vector<RenderingQueueVertex>  m_Vertexes      {std::vector<RenderingQueueVertex>()};
            std::vector<int>                   m_Indexes       {std::vector<int>()};
            gs_rectf                           m_Viewport      {-gs_huge<float>(), -gs_huge<float>(), +gs_huge<float>(), +gs_huge<float>()}; 
            RenderingQueueShader               m_DefaultShader {RenderingQueueShader()};
            RenderingQueueTexture              m_DefaultTexture{RenderingQueueTexture()};
            RenderingQueueFont                 m_DefaultFont   {RenderingQueueFont()};
        };
    }
}