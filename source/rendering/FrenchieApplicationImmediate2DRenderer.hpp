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

            // static API
            static gs_rectf calculate_text_bounding_box(
                const std::string&        _Text,
                const float&              _Size,
                const gs_vec2f&           _Position,
                const gs_mat4f&           _Transform,
                const RenderingQueueFont& _Font);

            static gs_rectf calculate_arc_bounding_box(
                const gs_vec2f& _Center,
                const float&    _MinorRadius,
                const float&    _MajorRadius,
                const float&    _SourceAngle,
                const float&    _TargetAngle,
                gs_mat4f&       _Transform);

            static gs_rectf calculate_triangle_bounding_box(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const gs_vec2f& _P3,
                const gs_mat4f& _Transform);

            static gs_rectf calculate_rectangle_bounding_box(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const gs_mat4f& _Transform);

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

            // this is a plipeline
            std::vector<RenderingQueueVertex>  m_Vertexes      {std::vector<RenderingQueueVertex>()};
            std::vector<int>                   m_Indexes       {std::vector<int>()};
            gs_rectf                           m_Viewport      {-gs_huge<float>(), -gs_huge<float>(), +gs_huge<float>(), +gs_huge<float>()}; 
            RenderingQueueShader               m_DefaultShader {RenderingQueueShader()};
            RenderingQueueTexture              m_DefaultTexture{RenderingQueueTexture()};
            RenderingQueueFont                 m_DefaultFont   {RenderingQueueFont()};
        };
    }
}