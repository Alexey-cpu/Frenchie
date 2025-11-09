#pragma once

#include <FrenchieApplication.hpp>

// STL
#include <queue>

namespace Frenchie
{
    namespace Application
    {
        class Renderer2D : public Layer
        {
        public:
            Renderer2D() : Layer(STRINGIFY(Renderer2D)){}
            virtual ~Renderer2D(){}

            virtual bool awake() override
            {
                m_Shader = Frenchie::Application::application_platform_backend_renderer()->construct_shader(
                    {
                        // Vertex shader
                        {
                            std::string(
R"(
#version 330 core

// vertex attributes
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_UV;

// outputs
out vec3 Normal;
out vec2 UV;

// uniforms
uniform mat4 u_ModelMatrix;
uniform mat4 u_ProjectionMatrix;

void main()
{
    // setup position
    gl_Position = u_ProjectionMatrix * u_ModelMatrix * vec4(a_Position, 1.0);

    // setup outputs
    Normal = a_Normal;
    UV     = a_UV;
}            
)"),
                            PlatformRendererBackendShaderType_::PlatformRendererBackendShaderType_Vertex
                        },

                        // fragment shader
                        {
                            std::string(
R"(
#version 330 core

//outputs
out vec4 fragColor;

//uniforms
uniform vec4 u_Color;

void main()
{
    // setup vertex color
    fragColor = u_Color;
}
)"),
                            PlatformRendererBackendShaderType_::PlatformRendererBackendShaderType_Fragment
                         },
                    }
                );

                m_Mesh = Frenchie::Application::application_platform_backend_renderer()->construct_mesh(
                    std::vector<PlatformRendererBackendMeshVertex>({
                    // trangle 1
                    { glm::vec3(-0.f, +0.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
                    { glm::vec3(+100.f, +0.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
                    { glm::vec3(-0.f, -100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
                    // // trangle 1
                    // { glm::vec3(-100.f, +100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
                    // { glm::vec3(+100.f, +100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
                    // { glm::vec3(-100.f, -100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },

                    // // trangle 2
                    // { glm::vec3(+100.f, +100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
                    // { glm::vec3(+100.f, -100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
                    // { glm::vec3(-100.f, -100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) }
                })
                );

                return true;
            }

            virtual void frame_start() override
            {
            }

            virtual void frame_update() override
            {
            }

            virtual void frame_render() override
            {
                // 2D projection example
                
                // screen size
                float width  = 2048.f;
                float height = 2048.f;

                // adjust 2D projection to start at top left corner
                float left   = -width  * 0.5f;// + width  * 0.5f; // The x-coordinate of the left edge of the viewable area.
                float right  = +width  * 0.5f;// + width  * 0.5f; // The x-coordinate of the right edge of the viewable area.
                float bottom = -height * 0.5f;// - height * 0.5f; // The y-coordinate of the bottom edge of the viewable area.
                float top    = +height * 0.5f;// - height * 0.5f; // The y-coordinate of the top edge of the viewable area.

                // create projection matrix
                auto projection = glm::ortho(left, right, bottom, top, -1000.0f, 1000.0f);
                auto view       = glm::mat4(1);

                Frenchie::Application::application_platform_backend_renderer()->begin_use_shader(m_Shader);
                
                Frenchie::Application::application_platform_backend_renderer()
                    ->set_shader_uniform(m_Shader, "u_ProjectionMatrix", projection * view);

                Frenchie::Application::application_platform_backend_renderer()
                    ->set_shader_uniform(m_Shader, "u_ModelMatrix", glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f)));

                Frenchie::Application::application_platform_backend_renderer()
                    ->set_shader_uniform(m_Shader, "u_Color", glm::vec4(255, 0, 0, 255));

                Frenchie::Application::application_platform_backend_renderer()->begin_render_mesh(
                    m_Mesh,
                    PlatformRendererBackendMeshRenderingHints_::PlatformRendererBackendMeshRenderingHints_Lines);

                Frenchie::Application::application_platform_backend_renderer()->endup_render_mesh();

                Frenchie::Application::application_platform_backend_renderer()->endup_use_shader();
            }

            virtual void frame_finish() override
            {
            }

            virtual void finish() override
            {
            }

            virtual void quit() override
            {
            }

            virtual bool allows_multiple_instances() const override
            {
                return false;
            }

        protected:
            std::shared_ptr<PlatformRendererBackendShader> m_Shader{nullptr};
            std::shared_ptr<PlatformRendererBackendMesh>   m_Mesh  {nullptr};
        };
    }
}