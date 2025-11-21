#pragma once

#include <FrenchieApplication.hpp>

namespace Frenchie
{
    namespace Application
    {
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

        class RenderingQueueTestLayer : public Layer
        {
        public:
            RenderingQueueTestLayer(){}
            virtual ~RenderingQueueTestLayer(){}

            virtual void frame_update() override
            {
                // compute shader projection matrix
                float width  = Frenchie::Application::application()->get_size()[0];
                float height = Frenchie::Application::application()->get_size()[1];
                float left   = -width  * 0.5f + width  * 0.5f; // The x-coordinate of the left edge of the viewable area.
                float right  = +width  * 0.5f + width  * 0.5f; // The x-coordinate of the right edge of the viewable area.
                float bottom = -height * 0.5f - height * 0.5f; // The y-coordinate of the bottom edge of the viewable area.
                float top    = +height * 0.5f - height * 0.5f; // The y-coordinate of the top edge of the viewable area.

                Frenchie::Application::application_rendering_queue()
                    ->set_projection_matrix(gs_matrix_ortho(left, right, bottom, top, -1000.0f, 1000.0f));

                Frenchie::Application::application_rendering_queue()
                    ->set_cameraview_matrix(gs_mat4f(1.f));

                if(Frenchie::Application::application()
                    ->is_mouse_button_down(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left))
                {   
                    auto x = Frenchie::Application::application()->get_cursor_position()[0];
                    auto y = Frenchie::Application::application()->get_cursor_position()[1];

                    Transform = gs_matrix_translate(gs_mat4f(1), gs_vec3f(x, -y, -100.f));
                }

                Frenchie::Application::application_rendering_queue()->PushTriangleFilled2D(
                    gs_vec3f(-0.f, +0.f, -500.f),
                    gs_vec3f(+100.f, +0.f, -500.f),
                    gs_vec3f(-0.f, -100.f, -500.f),
                    gs_vec4f(150.f, 32.f, 32.f, 128.f),
                    Transform
                );
            }
        
            gs_mat4f Transform = gs_matrix_translate(gs_mat4f(1), gs_vec3f(100.f, -100.f, -100.f));
        };
    }
}