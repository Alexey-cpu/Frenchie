#pragma once

#include <FrenchieApplication.hpp>

#include <chrono>
#include <iostream>

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
            void push_rendering_command(const gs_vec4f& _Color, const gs_mat4f& _Transform);

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
            static void build_triangle_filled_mesh(
                const gs_vec2f&                    _P1,
                const gs_vec2f&                    _P2,
                const gs_vec2f&                    _P3,
                const float&                       _Depth,
                const RenderingQueueTexture&       _Texture,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            static void build_rectangle_filled_mesh(
                const gs_vec2f&                    _Min,
                const gs_vec2f&                    _Max,
                const float&                       _Depth,
                const RenderingQueueTexture&       _Texture,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            static void build_line_mesh(
                const gs_vec2f&                    _P1,
                const gs_vec2f&                    _P2,
                const float&                       _Depth,
                const float&                       _LineWidth,
                const RenderingQueueTexture&       _Texture,
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
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            // this is a plipeline
            std::vector<RenderingQueueVertex>  m_Vertexes        {std::vector<RenderingQueueVertex>()};
            std::vector<int>                   m_Indexes         {std::vector<int>()};
            gs_rectf                           m_Viewport        {-gs_huge<float>(), -gs_huge<float>(), +gs_huge<float>(), +gs_huge<float>()}; 
            RenderingQueueShader               m_DefaultShader   {RenderingQueueShader()};
            RenderingQueueTexture              m_DefaultTexture  {RenderingQueueTexture()};
        };
    }
}

// template<typename Type> struct Tree;

// template<typename Type>
// struct Node
// {
//     int               Parent{-1};
//     int               Index {-1};
//     Type              Data  {Type()};
//     const Tree<Type>* Tree  {nullptr};
// };

// template<typename Type>
// struct Tree
// {
//     std::vector<Node<Type>> Nodes  {std::vector<Node<Type>>()};
//     std::vector<int>        Indexes{std::vector<int>()};
//     std::vector<int>        Entries{std::vector<int>()};
//     bool                    Dirty  {true};

//     template<typename ... Args>
//     Node<Type> construct_node(const Node<Type>& _Parent, Args ... _Args)
//     {
//         // make dirty
//         Dirty = true;

//         // create node
//         Node<Type> node;
//         node.Parent = _Parent.Index;
//         node.Index  = (int)Nodes.size();
//         node.Data   = Type(_Args ...);
//         node.Tree   = this;
//         Nodes.push_back(node);
//         return node;
//     }

//     void sort()
//     {
//         std::vector<Node<Type>> nodes(Nodes.size());
//         std::vector<int> workspace(Nodes.size()+1);

//         Indexes.resize(Nodes.size() + 1);
//         Entries.resize(Nodes.size());

//         for(int i = 0; i < Entries.size(); i++)
//         {
//             Entries[i] = 0;
//             Indexes[i] = 0;
//         }

//         // count items
//         for (int i = 0; i < Nodes.size(); i++)
//         {
//             if(Nodes[i].Parent < 0) continue;
//             ++Entries[Nodes[i].Parent];
//         }

//         // cumulative sum
//         int sum = 0;
//         for (int i = 0; i < Nodes.size(); i++)
//         {
//             Indexes  [i] = sum;
//             workspace[i] = sum;
//             sum += Entries[i];
//         }
//         Indexes[Nodes.size()] = sum;

//         for(int i = 0; i < Nodes.size(); i++ )
//         {
//             if(Nodes[i].Parent < 0) continue;
//             nodes[workspace[Nodes[i].Parent]++] = Nodes[i];
//         }

//         Nodes = nodes;
//     }

//     void clear()
//     {
//         Nodes.clear();
//     }
// };