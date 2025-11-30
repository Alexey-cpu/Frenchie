#pragma once

#include <FrenchieApplicationImmediate2DRenderer.hpp>

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

                return m_Renderer != nullptr;
            }

            virtual void frame_start() override
            {
                // add graphical primitives
                // if(Frenchie::Application::application()
                //     ->is_mouse_button_down(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left))
                // {
                //     auto x = Frenchie::Application::application()->get_window_cursor_position().x;
                //     auto y = Frenchie::Application::application()->get_window_cursor_position().y;

                //     Transform = gs_matrix_translate(
                //         gs_mat4f(1),
                //         gs_vec3f(
                //             x, 
                //             -y, 
                //             -100.f
                //         )
                //     );
                // }
            }

            virtual void frame_update() override
            {
                if(m_Renderer == nullptr)
                    return;

                draw_rectangle_filled();
                //draw_text();
                //draw_triangle_filled();

                // for(int i = 0; i < 12; ++i)
                // {
                //     for(int j = 0; j < 12; ++j)
                //     {
                //         float lineWidth = 8.f;
                //         float R = 128.f;

                //         // m_Renderer->push_triangle_filled(
                //         //     gs_vec2f(i * R * 2, j * R * 2),
                //         //     gs_vec2f(i * R * 2 + R, j * R * 2 + R),
                //         //     gs_vec2f(i * R * 2 + R, j * R * 2 - R),
                //         //     +0.f,
                //         //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //         //     Transform
                //         // );

                //         // m_Renderer->push_triangle(
                //         //     gs_vec2f(i * R * 2, j * R * 2),
                //         //     gs_vec2f(i * R * 2 + R, j * R * 2 + R),
                //         //     gs_vec2f(i * R * 2 + R, j * R * 2 - R),
                //         //     +0.f,
                //         //     lineWidth,
                //         //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //         //     Transform
                //         // );

                //         // m_Renderer->push_rectangle(
                //         //     gs_vec2f(i * R * 2, j * R * 2),
                //         //     gs_vec2f(i * R * 2 + R, j * R * 2 + R),
                //         //     +0.f,
                //         //     lineWidth,
                //         //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //         //     Transform);

                //         m_Renderer->push_arc_filled(
                //             gs_vec2f(i * R * 2, j * R * 2),
                //             +R,
                //             +R,
                //             0.f,
                //             360.f,
                //             0.f,
                //             gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //             Transform
                //         );

                //         // m_Renderer->push_rectangle_rounded(
                //         //     gs_vec2f(i * R * 2, j * R * 2),
                //         //     gs_vec2f(i * R * 2 + R, j * R * 2 + R),
                //         //     32,
                //         //     +0.f,
                //         //     lineWidth,
                //         //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //         //     Transform);

                //         // m_Renderer->push_rectangle_filled(
                //         //     gs_vec2f(i * R * 2, j * R * 2),
                //         //     gs_vec2f(i * R * 2 + R, j * R * 2 + R),
                //         //     +0.f,
                //         //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //         //     Transform);

                //         // m_Renderer->push_arc(
                //         //     gs_vec2f(i * R * 2, j * R * 2),
                //         //     +R,
                //         //     +R * 0.5f,
                //         //     0.f,
                //         //     360.f,
                //         //     0.f,
                //         //     lineWidth,
                //         //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //         //     Transform
                //         // );
                //     }
                // }
            }

            void draw_triangle_filled()
            {
                // m_Renderer->push_polygon(
                //     0.f,
                //     m_Renderer->m_DefaultTexture,
                //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //     Transform,
                //     gs_vec2f(0.f, -150.f),
                //     gs_vec2f(-150.f, +0.f),
                //     gs_vec2f(-75.f, +75.f),
                //     gs_vec2f(-150.f, +150.f),
                //     gs_vec2f(+150.f, +150.f)
                // );

                m_Renderer->push_polygon(
                    0.f,
                    m_Renderer->m_DefaultTexture,
                    gs_vec4f(255.f, 0.f, 0.f, 255.f),
                    Transform,
                    gs_vec2f(0.f, -100.f),
                    gs_vec2f(-50.f, +0.f),
                    gs_vec2f(-100.f, +100.f),
                    gs_vec2f(0.f, +150.f),
                    gs_vec2f(+50.f, +100.f),
                    gs_vec2f(+0.f, +50.f),
                    gs_vec2f(+50.f, +0.f),
                    gs_vec2f(+50.f, -50.f)
                );

                // m_Renderer->push_polygon(
                //     0.f,
                //     m_Renderer->m_DefaultTexture,
                //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //     Transform,
                //     gs_vec2f(0.f, 0.f),
                //     gs_vec2f(0.f, +50.f),
                //     gs_vec2f(+50.f, +50.f),
                //     gs_vec2f(+50.f, +0.f)
                // );

                // auto bbox = m_Renderer->calculate_bounding_box(
                //     Transform,
                //     gs_vec2f(0.f, 0.f),
                //     gs_vec2f(+75.f, +150.f),
                //     gs_vec2f(+450.f, -150.f)
                // );

                // m_Renderer->push_triangle_filled(
                //     gs_vec2f(0.f, 0.f),
                //     gs_vec2f(+50.f, +50.f),
                //     gs_vec2f(0, +50.f),
                //     +0.f,
                //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //     Transform
                // );

                // m_Renderer->push_rectangle(
                //     bbox.Min,
                //     bbox.Max,
                //     -100.f,
                //     12.f,
                //     gs_vec4f(0.f, 255.f, 0.f, 255.f),
                //     gs_mat4f(1.f)
                // );
            }

            void draw_rectangle_filled()
            {
                m_Renderer->push_rectangle_filled(
                    gs_vec2f(0.f, -0.f),
                    gs_vec2f(128.f, -128.f),
                    +100.f,
                    gs_vec4f(255.f, 0.f, 0.f, 32.f),
                    Transform);

                m_Renderer->push_rectangle_filled(
                    gs_vec2f(128.f, -128.f),
                    gs_vec2f(128.f + 64.f, -128.f -64.f),
                    +100.f,
                    gs_vec4f(0.f, 255.f, 0.f, 12.f),
                    Transform);
            }

            void draw_text()
            {
                auto text = "Всем привет, меня зовут Алексей !!!";
                
                auto x = Frenchie::Application::application()->get_window_size().x;
                auto y = Frenchie::Application::application()->get_window_size().y;

                gs_vec2f position = gs_vec2f(x * 0.5f, -y * 0.5f) ;

                float size = 32.f;

                build_push_button_mesh(
                    text,
                    32.f,
                    position,
                    0.f,
                    gs_mat4f(1.f));
            }

        protected:

            enum ImmedidateUserInterfaceButtonColors
            {
                // enabled
                ImmedidateUserInterfaceButton_Enabled,
                ImmedidateUserInterfaceButton_EnabledPushed,
                ImmedidateUserInterfaceButton_EnabledHovered,
                ImmedidateUserInterfaceButton_EnabledFocused,
                
                // disabled
                ImmedidateUserInterfaceButton_Disabled,
                ImmedidateUserInterfaceButton_DisabledPushed,
                ImmedidateUserInterfaceButton_DisabledHovered,
                ImmedidateUserInterfaceButton_DisabledFocused,
            };

            void build_push_button_mesh(
                const std::string& _Name,
                const float&       _FontSize,
                const gs_vec2f&    _Position,
                const float&       _Depth,
                const gs_mat4f&    _Transform)
            {
                auto Hovered = gs_vec4f(0.f, 0.f, 255.f, 32.f);
                auto Pushed  = gs_vec4f(128.f, 128.f, 0.f, 64.f);
                auto Default = gs_vec4f(255.f, 0.f, 0.f, 64.f);

                // compute text bounding box
                gs_rectf textBoundingBox =
                    m_Renderer->calculate_utf8_text_bounding_box(
                        _Name,
                        _FontSize,
                        _Position,
                        _Transform,
                        m_Renderer->m_DefaultFont);

                auto Color = Default;

                auto x = Frenchie::Application::application()->get_window_cursor_position().x;
                auto y = Frenchie::Application::application()->get_window_cursor_position().y;

                if(textBoundingBox.contains(gs_vec2f(x, -y)))
                {
                    if(Frenchie::Application::application()->is_mouse_button_pressed(ApplicationMouseButton::Button::ApplicationMouseButton_Left))
                    {
                        Color = Pushed;
                    }
                    else
                    {
                        Color = Hovered;

                        std::cout << "textBoundingBox.contains " << x << "\t" << -y << "\n";
                    }
                }

                m_Renderer->push_rectangle_filled(
                    gs_vec2f(x, -y),
                    gs_vec2f(x, -y) + gs_vec2f(16.f, 16.f),
                    _Depth,
                    Color,
                    _Transform);

                m_Renderer->push_rectangle_filled(
                    textBoundingBox.Min,
                    textBoundingBox.Max,
                    _Depth,
                    Color,
                    _Transform);

                // m_Renderer->push_utf8_text(
                //     _Name,
                //     _FontSize,
                //     _Position,
                //     _Depth + 1,
                //     gs_vec4f(0.f, 255.f ,0.f, 128.f),
                //     _Transform);
            }

            gs_mat4f Transform = gs_mat4f(1.f);
            std::shared_ptr<Immediate2DRenderer> m_Renderer{nullptr};

        };
    }
}