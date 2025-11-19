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
                // create texture
                // const int textHeight   = 4;
                // const int textWidth    = 4;
                // const int textChannels = 4;
                // unsigned char image[textWidth * textHeight * textChannels]{};

                // // colorify text
                // int red   = 0;
                // int green = 1;
                // int blue  = 2;
                // int alpha = 3;

                // for (int y = 0; y < textHeight; y++)
                // {
                //     for (int x = 0; x < textWidth; x++)
                //     {
                //         image[textChannels * (y * textWidth + x) + red  ] = 255;
                //         image[textChannels * (y * textWidth + x) + green] = 255;
                //         image[textChannels * (y * textWidth + x) + blue ] = 255;
                //         image[textChannels * (y * textWidth + x) + alpha] = 255;
                //     }
                // }
                
                
                m_Texture = Frenchie::Application::application_platform_backend_renderer()->construct_image(
                    "C:/SDK/Qt_Projects/OpenGL/logs/images/image.png");

                // create shader
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

// inputs
in vec2 UV;

//outputs
out vec4 fragColor;

//uniforms
uniform vec4      u_Color;
uniform sampler2D u_Texture;

void main()
{
    // setup vertex color
    fragColor = texture(u_Texture, UV);
}
)"),
                            PlatformRendererBackendShaderType_::PlatformRendererBackendShaderType_Fragment
                         },
                    }
                );

                return true;
            }

            virtual void frame_start() override
            {
                // compute shader projection matrix
                float width  = Frenchie::Application::application_platform_backend_renderer()->get_context_window_size().x;
                float height = Frenchie::Application::application_platform_backend_renderer()->get_context_window_size().y;
                float left   = -width  * 0.5f;// + width  * 0.5f; // The x-coordinate of the left edge of the viewable area.
                float right  = +width  * 0.5f;// + width  * 0.5f; // The x-coordinate of the right edge of the viewable area.
                float bottom = -height * 0.5f;// - height * 0.5f; // The y-coordinate of the bottom edge of the viewable area.
                float top    = +height * 0.5f;// - height * 0.5f; // The y-coordinate of the top edge of the viewable area.

                m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1000.0f, 1000.0f);
                m_CameraViewMatrix = glm::mat4(1);

                // setup shader projection matrix
                Frenchie::Application::application_platform_backend_renderer()->begin_use_shader(m_Shader);
                Frenchie::Application::application_platform_backend_renderer()->set_shader_uniform(m_Shader, "u_ProjectionMatrix", m_ProjectionMatrix * m_CameraViewMatrix);
                Frenchie::Application::application_platform_backend_renderer()->end_use_shader();
            }

            virtual void frame_update() override
            {
                // AddLine(
                //     glm::vec3(-0.f, +0.f, -20.f),
                //     glm::vec3(+100.f, +0.f, -20.f),
                //     glm::vec4(0, 255, 0, 255),
                //     glm::mat4(1));

                AddTriangle(
                    glm::vec3(-0.f, +0.f, -500.f),
                    glm::vec3(+100.f, +0.f, -500.f),
                    glm::vec3(-0.f, -100.f, -500.f),
                    glm::vec4(0, 0, 255, 255),
                    glm::mat4(1));

                AddTriangleFilled(
                    glm::vec3(-0.f, +0.f, 0.f),
                    glm::vec3(+100.f, +0.f, 0.f),
                    glm::vec3(-0.f, -100.f, 0.f),
                    glm::vec4(255, 0, 0, 255),
                    glm::translate(glm::mat4(1), glm::vec3(5.f, 5.f, 0.f)));
            }

            virtual void frame_render() override
            {
                for(auto&& command : m_Commands)
                {
                    // render mesh
                    Frenchie::Application::application_platform_backend_renderer()->begin_use_shader(m_Shader);
                    Frenchie::Application::application_platform_backend_renderer()->set_shader_uniform(m_Shader, "u_ModelMatrix", command.Transform);
                    Frenchie::Application::application_platform_backend_renderer()->set_shader_uniform(m_Shader, "u_Color", command.Color);
                    Frenchie::Application::application_platform_backend_renderer()->set_shader_uniform(m_Shader, "u_Texture", 0);

                    Frenchie::Application::application_platform_backend_renderer()->bind_image(command.Texture);

                    Frenchie::Application::application_platform_backend_renderer()->begin_render_mesh(command.Mesh, command.RendererHints);
                    Frenchie::Application::application_platform_backend_renderer()->endup_render_mesh();
                    Frenchie::Application::application_platform_backend_renderer()->end_use_shader();

                    // destroy mesh
                    Frenchie::Application::application_platform_backend_renderer()->destroy_mesh(command.Mesh);
                }

                // clear commands list
                m_Commands.clear();
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

            // nested types
            struct RendererCommand
            {
                RendererCommand(
                    const PlatformRendererBackendMesh&               _Mesh          = PlatformRendererBackendMesh(),
                    const PlatformRendererBackendShader&             _Shader        = PlatformRendererBackendShader(),
                    const glm::mat4&                                 _Transform     = glm::mat4(1.f),
                    const glm::vec4&                                 _Color         = glm::vec4(255, 255, 255, 255),
                    const PlatformRendererBackendTexture&            _Texture       = PlatformRendererBackendTexture(),
                    const PlatformRendererBackendMeshRenderingHints& _RendererHints = PlatformRendererBackendMeshRenderingHints_::PlatformRendererBackendMeshRenderingHints_Default) :
                    Mesh(_Mesh),
                    Shader(_Shader),
                    Transform(_Transform),
                    Color(_Color),
                    Texture(_Texture),
                    RendererHints(_RendererHints){}

                PlatformRendererBackendMesh               Mesh;
                PlatformRendererBackendShader             Shader;
                glm::mat4                                 Transform;
                glm::vec4                                 Color;
                PlatformRendererBackendTexture            Texture;
                PlatformRendererBackendMeshRenderingHints RendererHints;
            };

            void AddTriangle(glm::vec3 _P1, glm::vec3 _P2, glm::vec3 _P3, glm::vec4 _Color, glm::mat4 _Transform)
            {
                // constgruct mesh
                PlatformRendererBackendMeshVertex vertexes[3] = {PlatformRendererBackendMeshVertex(_P1), PlatformRendererBackendMeshVertex(_P2), PlatformRendererBackendMeshVertex(_P3)};
                int                               indexes [3] = {0, 1, 2};

                m_Commands.push_back(
                    RendererCommand(
                        Frenchie::Application::application_platform_backend_renderer()->construct_mesh(&vertexes[0], 3, &indexes[0], 3),
                        m_Shader,
                        _Transform,
                        _Color,
                        PlatformRendererBackendTexture(),
                        PlatformRendererBackendMeshRenderingHints_::PlatformRendererBackendMeshRenderingHints_Lines
                    )
                );
            }

            void AddTriangleFilled(glm::vec3 _P1, glm::vec3 _P2, glm::vec3 _P3, glm::vec4 _Color, glm::mat4 _Transform)
            {
                // constgruct mesh
                PlatformRendererBackendMeshVertex vertexes[3] = 
                    {
                        PlatformRendererBackendMeshVertex(_P1, glm::vec3(0), glm::vec2(_P1.x / m_Texture.Width, _P1.y / m_Texture.Height)),
                        PlatformRendererBackendMeshVertex(_P2, glm::vec3(0), glm::vec2(_P2.x / m_Texture.Width, _P2.y / m_Texture.Height)),
                        PlatformRendererBackendMeshVertex(_P3, glm::vec3(0), glm::vec2(_P3.x / m_Texture.Width, _P3.y / m_Texture.Height))
                    };
                int indexes [3] = {0, 1, 2};

                m_Commands.push_back(
                    RendererCommand(
                        Frenchie::Application::application_platform_backend_renderer()->construct_mesh(&vertexes[0], 3, &indexes[0], 3),
                        m_Shader,
                        _Transform,
                        _Color,
                        m_Texture,
                        PlatformRendererBackendMeshRenderingHints_::PlatformRendererBackendMeshRenderingHints_Triangles
                    )
                );
            }

            void AddLine(glm::vec3 _P1, glm::vec3 _P2, glm::vec4 _Color, glm::mat4 _Transform)
            {
                // constgruct mesh
                PlatformRendererBackendMeshVertex vertexes[2] = {PlatformRendererBackendMeshVertex(_P1), PlatformRendererBackendMeshVertex(_P2)};
                int                               indexes [2] = {0, 1};

                m_Commands.push_back(
                    RendererCommand(
                        Frenchie::Application::application_platform_backend_renderer()->construct_mesh(&vertexes[0], 3, &indexes[0], 3),
                        m_Shader,
                        _Transform,
                        _Color,
                        PlatformRendererBackendTexture(),
                        PlatformRendererBackendMeshRenderingHints_::PlatformRendererBackendMeshRenderingHints_Lines
                    )
                );
            }

            glm::mat4                      m_ProjectionMatrix;
            glm::mat4                      m_CameraViewMatrix;
            PlatformRendererBackendShader  m_Shader;
            PlatformRendererBackendTexture m_Texture;
            std::vector<RendererCommand>   m_Commands;
        };
    }
}