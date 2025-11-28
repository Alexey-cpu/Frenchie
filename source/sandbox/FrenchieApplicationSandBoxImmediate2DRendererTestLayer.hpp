#pragma once

#include <FrenchieApplicationImmediate2DRenderer.hpp>

namespace Frenchie
{
    namespace Application
    {
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
                if(Frenchie::Application::application()
                    ->is_mouse_button_down(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left))
                {
                    auto x = Frenchie::Application::application()->get_window_cursor_position().x;
                    auto y = Frenchie::Application::application()->get_window_cursor_position().y;

                    Transform = gs_matrix_translate(
                        gs_mat4f(1),
                        gs_vec3f(
                            x, 
                            -y, 
                            -100.f
                        )
                    );
                }
            }

            virtual void frame_update() override
            {
                if(m_Renderer == nullptr)
                    return;

                //draw_text();
                draw_triangle_filled();

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

                // m_Renderer->push_polygon(
                //     0.f,
                //     m_Renderer->m_DefaultTexture,
                //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //     Transform,
                //     gs_vec2f(0.f, -100.f),
                //     gs_vec2f(-50.f, +0.f),
                //     gs_vec2f(-100.f, +100.f),
                //     gs_vec2f(0.f, +150.f),
                //     gs_vec2f(+50.f, +100.f),
                //     gs_vec2f(+0.f, +50.f),
                //     gs_vec2f(+50.f, +0.f),
                //     gs_vec2f(+50.f, -50.f)
                // );

                m_Renderer->push_polygon(
                    0.f,
                    m_Renderer->m_DefaultTexture,
                    gs_vec4f(255.f, 0.f, 0.f, 255.f),
                    Transform,
                    gs_vec2f(0.f, 0.f),
                    gs_vec2f(0.f, +50.f),
                    gs_vec2f(+50.f, +50.f),
                    gs_vec2f(+50.f, +0.f)
                );

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
                    gs_vec4f(255.f, 0.f, 0.f, 255.f),
                    Transform);

                m_Renderer->push_rectangle_filled(
                    gs_vec2f(128.f, -128.f),
                    gs_vec2f(128.f + 64.f, -128.f -64.f),
                    +100.f,
                    gs_vec4f(0.f, 255.f, 0.f, 255.f),
                    Transform);
            }

            void draw_text()
            {
                auto text = "Hello world\n\tMy name is Alexey =)";
                
                m_Renderer->push_text(text, 128.f, gs_vec2f(0.f, 0.f), gs_vec4f(255.f, 0.f ,0.f, 128.f), Transform);

                auto rect = m_Renderer->calculate_text_bounding_box(text, 128.f, gs_vec2f(0.f, 0.f), Transform, m_Renderer->m_DefaultFont);

                m_Renderer->push_rectangle_rounded(
                    rect.Min,
                    rect.Max,
                    32,
                    +0.f,
                    12.f,
                    gs_vec4f(255.f, 0.f, 0.f, 255.f),
                    gs_mat4f(1.f));
            }

        protected:

            gs_mat4f Transform = gs_mat4f(1.f);

            std::shared_ptr<Immediate2DRenderer> m_Renderer{nullptr};

        };
    }
}