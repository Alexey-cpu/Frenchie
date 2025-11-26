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
                    auto x = Frenchie::Application::application()->get_cursor_position().x;
                    auto y = Frenchie::Application::application()->get_cursor_position().y;

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

                // m_Renderer->push_rectangle_filled(
                //     gs_vec2f(0.f, 0.f),
                //     gs_vec2f(m_Renderer->m_Font.AtlasTexture.Width, -m_Renderer->m_Font.AtlasTexture.Height),
                //     +0.f,
                //     gs_vec4f(255.f, 255.f, 255.f, 255.f),
                //     Transform, 
                //     m_Renderer->m_Font.AtlasTexture);

                m_Renderer->push_text(m_Renderer->m_Font, Transform);

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

                //         m_Renderer->push_rectangle_rounded(
                //             gs_vec2f(i * R * 2, j * R * 2),
                //             gs_vec2f(i * R * 2 + R, j * R * 2 + R),
                //             32,
                //             +0.f,
                //             lineWidth,
                //             gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //             Transform);

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
                //}
            }

        protected:

            gs_mat4f Transform = gs_mat4f(1.f);

            std::shared_ptr<Immediate2DRenderer> m_Renderer{nullptr};

        };
    }
}