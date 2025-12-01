#pragma once

#include <FrenchieApplicationImmediate2DRenderer.hpp>

// STL
#include <chrono>

template<typename Type> struct Tree;

template<typename Type>
struct Node
{
    int               Parent{-1};
    int               Index {-1};
    Type              Data  {Type()};
    const Tree<Type>* Tree  {nullptr};
};

template<typename Type>
struct Tree
{
    mutable std::vector<Node<Type>> Nodes  {std::vector<Node<Type>>()};
    mutable std::vector<int>        Indexes{std::vector<int>()};
    mutable std::vector<int>        Entries{std::vector<int>()};
    mutable bool                    Dirty  {true};

    template<typename ... Args>
    Node<Type> construct_node(const Node<Type>& _Parent, Args ... _Args)
    {
        Node<Type> node;
        node.Parent = _Parent.Index;
        node.Index  = (int)Nodes.size();
        node.Data   = Type(_Args ...);
        node.Tree   = this;
        Nodes.push_back(node);
        Dirty = true;
        return node;
    }

    void clear()
    {
        Nodes.clear();
    }

    void sort() const
    {
        std::vector<Node<Type>> nodes(Nodes.size());
        std::vector<int> workspace(Nodes.size()+1);

        Indexes.resize(Nodes.size() + 1);
        Entries.resize(Nodes.size());

        for(int i = 0; i < Entries.size(); i++)
        {
            Entries[i] = 0;
            Indexes[i] = 0;
        }

        // count items
        for (int i = 0; i < Nodes.size(); i++)
        {
            if(Nodes[i].Parent < 0) continue;
            ++Entries[Nodes[i].Parent];
        }

        // cumulative sum
        int sum = 0;
        for (int i = 0; i < Nodes.size(); i++)
        {
            Indexes  [i] = sum;
            workspace[i] = sum;
            sum += Entries[i];
        }
        Indexes[Nodes.size()] = sum;

        for(int i = 0; i < Nodes.size(); i++ )
        {
            if(Nodes[i].Parent < 0) continue;
            nodes[workspace[Nodes[i].Parent]++] = Nodes[i];
        }

        Nodes = nodes;
        Dirty = false;
    }
};

namespace Frenchie
{
    namespace Application
    {
        class ImmedidateUserInterfaceEntry
        {
            ImmedidateUserInterfaceEntry(const gs_rectf& _BoundingBox) :
                BoundingBox(_BoundingBox){}
            
            gs_rectf BoundingBox  {gs_rectf(0.f, 0.f, 128.f, 128.f)};
            gs_mat4f Transform    {gs_mat4f(1.f)};
            bool     Hovered      {false};
            bool     Clicked      {false};
            bool     DoubleClicked{false};

            void (*DrawFunction)(
                const char*     _Name,
                const gs_vec2f& _Position,
                const gs_vec2f& _Size,
                const gs_mat4f& _Transform) = nullptr;
        };

        class Immedidate2DRendererTestLayer : public Layer
        {
        public:
            Immedidate2DRendererTestLayer(){}
            virtual ~Immedidate2DRendererTestLayer(){}

            virtual bool awake() override
            {
                if(m_Renderer == nullptr)
                {
                    std::cout << "Immedidate2DRendererTestLayer...\n";
                    m_Renderer = Frenchie::Application::application()->push_layer<Immediate2DRenderer>();
                }

                AwakeTime = std::chrono::high_resolution_clock::now();

                return m_Renderer != nullptr;
            }

            virtual void frame_start() override
            {
                // add graphical primitives
                if(Frenchie::Application::application()
                    ->is_mouse_button_down(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left))
                {
                    auto size     = Frenchie::Application::application()->get_window_size();
                    auto position = Frenchie::Application::application()->get_window_cursor_position();

                    float ndc_x = (2.0f * position.x) / size.x - 1.0f;
                    float ndc_y = 1.0f - (2.0f * position.y) / size.y;

                    Position =
                        gs_matrix_invert_square(Frenchie::Application::application_rendering_queue()->get_projection_matrix()) *
                        gs_matrix_invert_square(Frenchie::Application::application_rendering_queue()->get_cameraview_matrix()) *
                        gs_vec4f(ndc_x, ndc_y, 0, 1.f);
                }

                Time = std::chrono::high_resolution_clock::now();
            }

            virtual void frame_update() override
            {
                if(m_Renderer == nullptr)
                    return;

                m_Renderer->push_rectangle(
                    m_Renderer->m_Viewport.Min,
                    m_Renderer->m_Viewport.Max,
                    12.f,
                    gs_vec4f(0.f, 255.f, 0.f, 255.f),
                    0.f
                );

                //draw_triangle_filled();
                //draw_rectangle_filled();
                draw_text();
                //draw_arc_filled();
                //draw_line();
                //draw_arc();
                //draw_rectangle();
                //draw_rectangle_rounded();
            }

            void draw_triangle_filled()
            {
                m_Renderer->push_triangle_filled(
                    gs_vec2f(0.f, 0.f),
                    gs_vec2f(+150.f, +150.f),
                    gs_vec2f(+150.f, -150.f),
                    gs_vec4f(255.f, 255.f, 255.f, 255.f),
                    +0.f,
                    Position,
                    (float)(PI2 * 50.f * (float)std::chrono::duration_cast<std::chrono::milliseconds>(Time - AwakeTime).count() / 1000.f),
                    gs_vec2f(1.5, 2.5)
                );
            }

            void draw_rectangle_filled()
            {
                m_Renderer->push_rectangle_filled(
                    gs_vec2f(0.f, 0.f),
                    gs_vec2f(+150.f, +150.f),
                    gs_vec4f(255.f, 255.f, 255.f, 255.f),
                    +0.f,
                    Position,
                    (float)(PI2 * 50.f * (float)std::chrono::duration_cast<std::chrono::milliseconds>(Time - AwakeTime).count() / 1000.f),
                    gs_vec2f(1.5, 2.5)
                );
            }

            void draw_text()
            {
                std::string text = "Hello. My name is Alexey\n\t and im very glad to see you )";

                float    lineWidth = 8.f;
                gs_vec4f lineColor = gs_vec4f(255.f, 32.f, 32.f, 128.f);
                float    lineAngle = 0.f;//(float)(PI2 * 50.f * (float)std::chrono::duration_cast<std::chrono::milliseconds>(Time - AwakeTime).count() / 1000.f);
                gs_vec2f scale     = gs_vec2f(1.f, 1.f);

                auto textBox = m_Renderer->calculate_bounding_box(
                    +0.f,
                    Position,
                    lineAngle,
                    scale,
                    text,
                    32.f,
                    m_Renderer->m_DefaultFont
                );

                m_Renderer->push_rectangle(
                    textBox.Min - lineWidth,
                    textBox.Max + lineWidth,
                    lineWidth,
                    gs_vec4f(255.f, 255.f, 255.f, 255.f),
                    +0.f
                );

                m_Renderer->push_utf8_text(
                    text,
                    32.f,
                    gs_vec4f(255.f, 0.f, 0.f, 128.f),
                    +0.f,
                    Position,
                    lineAngle,
                    scale
                );
            }

            void draw_arc_filled()
            {
                m_Renderer->push_arc_filled(
                    gs_vec2f(0.f, 0.f),
                    128.f,
                    256.f,
                    0.f,
                    270.f,
                    gs_vec4f(255.f, 32.f, 32.f, 128.f),
                    +0.f,
                    Position,
                    (float)(PI2 * 50.f * (float)std::chrono::duration_cast<std::chrono::milliseconds>(Time - AwakeTime).count() / 1000.f),
                    gs_vec2f(1.5, 2.5)
                );
            }

            void draw_line()
            {
                float    lineWidth = 16.f;
                gs_vec4f lineColor = gs_vec4f(255.f, 32.f, 32.f, 128.f);
                float    lineAngle = 0.f;// (float)(PI2 * 50.f * (float)std::chrono::duration_cast<std::chrono::milliseconds>(Time - AwakeTime).count() / 1000.f);

                m_Renderer->push_line(
                    gs_vec2f(0.f, 0.f),
                    gs_vec2f(150.f, 150.f),
                    lineWidth,
                    lineColor,
                    +0.f,
                    Position,
                    lineAngle,
                    gs_vec2f(1.5, 2.5)
                );

                m_Renderer->push_line(
                    gs_vec2f(150.f, 150.f),
                    gs_vec2f(150.f, -150.f),
                    lineWidth,
                    lineColor,
                    +0.f,
                    Position,
                    lineAngle,
                    gs_vec2f(1.5, 2.5)
                );
            }

            void draw_arc()
            {
                float lineWidth = 16.f;

                m_Renderer->push_arc(
                    gs_vec2f(0.f, 0.f),
                    128.f,
                    256.f,
                    0.f,
                    270.f,
                    lineWidth,
                    gs_vec4f(255.f, 32.f, 32.f, 128.f),
                    +0.f,
                    Position,
                    (float)(PI2 * 50.f * (float)std::chrono::duration_cast<std::chrono::milliseconds>(Time - AwakeTime).count() / 1000.f),
                    gs_vec2f(1.5, 2.5)
                );
            }

            void draw_rectangle()
            {
                float lineWidth = 16.f;
                
                m_Renderer->push_rectangle(
                    gs_vec2f(0.f, 0.f),
                    gs_vec2f(+150.f, +150.f),
                    lineWidth,
                    gs_vec4f(255.f, 255.f, 255.f, 255.f),
                    +0.f,
                    Position,
                    0.f,//(float)(PI2 * 50.f * (float)std::chrono::duration_cast<std::chrono::milliseconds>(Time - AwakeTime).count() / 1000.f),
                    gs_vec2f(1.5, 2.5)
                );
            }

            void draw_rectangle_rounded()
            {
                float lineWidth = 16.f;

                m_Renderer->push_rectangle_rounded(
                    gs_vec2f(0.f, 0.f),
                    gs_vec2f(+150.f, +150.f),
                    32.f,
                    lineWidth,
                    gs_vec4f(255.f, 255.f, 255.f, 255.f),
                    +0.f,
                    Position,
                    (float)(PI2 * 50.f * (float)std::chrono::duration_cast<std::chrono::milliseconds>(Time - AwakeTime).count() / 1000.f),
                    gs_vec2f(1.5, 2.5)
                );
            }

        protected:

            // enum ImmedidateUserInterfaceButtonColors
            // {
            //     // enabled
            //     ImmedidateUserInterfaceButton_Enabled,
            //     ImmedidateUserInterfaceButton_EnabledPushed,
            //     ImmedidateUserInterfaceButton_EnabledHovered,
            //     ImmedidateUserInterfaceButton_EnabledFocused,
                
            //     // disabled
            //     ImmedidateUserInterfaceButton_Disabled,
            //     ImmedidateUserInterfaceButton_DisabledPushed,
            //     ImmedidateUserInterfaceButton_DisabledHovered,
            //     ImmedidateUserInterfaceButton_DisabledFocused,
            // };

            // void build_push_button_mesh(
            //     const std::string& _Name,
            //     const float&       _FontSize,
            //     const gs_vec2f&    _Position,
            //     const float&       _Depth,
            //     const gs_mat4f&    _Transform)
            // {
            //     auto Hovered = gs_vec4f(0.f, 0.f, 255.f, 32.f);
            //     auto Pushed  = gs_vec4f(0.f, 255.f, 0.f, 255.f);
            //     auto Default = gs_vec4f(255.f, 0.f, 0.f, 255.f);

            //     // compute text bounding box
            //     gs_rectf textBoundingBox =
            //         m_Renderer->calculate_utf8_text_bounding_box(
            //             _Name,
            //             _FontSize,
            //             _Position,
            //             _Transform,
            //             m_Renderer->m_DefaultFont);

            //     auto Color = Default;

            //     auto x = Frenchie::Application::application()->get_window_cursor_position().x;
            //     auto y = Frenchie::Application::application()->get_window_cursor_position().y;

            //     if(textBoundingBox.contains(gs_vec2f(x, -y)))
            //     {
            //         if(Frenchie::Application::application()->is_mouse_button_pressed(ApplicationMouseButton::Button::ApplicationMouseButton_Left))
            //         {
            //             Color = Pushed;
            //         }
            //         else
            //         {
            //             Color = Hovered;
            //         }
            //     }

            //     m_Renderer->push_rectangle_filled(
            //         textBoundingBox.Min,
            //         textBoundingBox.Max,
            //         _Depth,
            //         Color,
            //         gs_mat4f(1.f));

            //     m_Renderer->push_rectangle_filled(
            //         textBoundingBox.Min,
            //         textBoundingBox.Max,
            //         _Depth,
            //         gs_vec4f(0.f, 255.f, 0.f, 255.f),
            //         gs_mat4f(1.f));
            // }

            gs_vec2f Position;
            std::chrono::high_resolution_clock::time_point  Time;
            std::chrono::high_resolution_clock::time_point   AwakeTime;

            //gs_mat4f Transform = gs_mat4f(1.f);
            std::shared_ptr<Immediate2DRenderer> m_Renderer{nullptr};

        };
    }
}