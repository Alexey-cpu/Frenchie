#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplication.hpp>

#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreObject.hpp>

#include <FrenchieRendererOpenGLCamera.hpp>
#include <FrenchieRendererOpenGLShaderProgram.hpp>
#include <FrenchieRendererOpenGLRectTransform.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Test
        {
            using namespace Frenchie::Core;
            using namespace Frenchie::Application;
            using namespace Frenchie::Renderer::OpenGL;

            struct Vertex
            {
                glm::vec3 Position;
                glm::vec3 Normal;
                glm::vec2 UV;
            };

            class RenderingTest : public Layer
            {
            public:

                RenderingTest(const std::string& _Name = "RenderingTest") : Layer(_Name){}
                
                virtual ~RenderingTest()
                {
                    // glDeleteBuffers(1, &m_VBO);
                    // glDeleteBuffers(1, &m_EBO);
                    // glDeleteVertexArrays(1, &m_VAO);
                }

                virtual bool awake() override
                {
                    setup_mesh();
                    load_shaders();
                    return true;
                }
                
                virtual void frame_start() override
                {
                    m_RectTransform->frame_start();
                }
                
                virtual void frame_update() override
                {
                    // compute geometry here
                    m_RectTransform->frame_update();
                }
                
                virtual void frame_finish() override
                {
                    // create world
                    Viewport viewport = Viewport(
                        100.f,
                        1.f,
                        90.f,
                        glm::vec3(1.f, 1.f, 1.f)
                    );

                    // create camera
                    Camera camera = Camera(
                        glm::vec3(+0.f, +0.f, +1.f),
                        glm::vec3(+0.f, +1.f, +0.f));

                    glm::mat4 modelMatrix;
                    glm::mat4 viewMatrix;
                    glm::mat4 projectionMatrix;
                    glm::vec3 viewportScale;

                    // compute model matrix
                    {
                        modelMatrix = m_RectTransform->get_model_matrix();//glm::rotate(glm::mat4(1.0f), 0.f, glm::vec3(1.0f, 0.0f, 0.0f));
                    }

                    // compute view matrix (camera matrix)
                    {
                        viewMatrix = camera.get_view_matrix(viewport);
                    }

                    // compute projection matrix
                    {
                        projectionMatrix = viewport.get_projection_matrix();
                    }

                    // compute world view port scale
                    {
                        viewportScale = viewport.get_viewport_scale(Frenchie::Application::Application::instance()->get_window_size());
                    }

                    // draw here
                    m_ShaderProgram->begin();
                    m_ShaderProgram->set_uniform<glm::vec3>("u_ViewportScale", viewportScale);
                    m_ShaderProgram->set_uniform<glm::mat4>("u_ModelMatrix", modelMatrix);
                    m_ShaderProgram->set_uniform<glm::mat4>("u_ViewMatrix", viewMatrix);
                    m_ShaderProgram->set_uniform<glm::mat4>("u_ProjectionMatrix", projectionMatrix);

                    m_ShaderProgram->set_uniform<glm::vec4>("u_Color", glm::vec4(0.5f, 0.5f, 0.5f, 1.f));

                    m_RectTransform->frame_finish();

                    m_ShaderProgram->end();

                    float angle = 2.f * glm::pi<float>() * 1.f * (float)glfwGetTime();

                    m_RectTransform->set_rotation(glm::vec3(glm::degrees(angle), 0.f, 0.f));
                }
                
                virtual void finish() override
                {
                }

            protected:
                std::shared_ptr<ShaderProgram> m_ShaderProgram = nullptr;
                RectTransform* m_RectTransform = nullptr;

                void setup_mesh(){}

                void load_shaders()
                {
                    m_ShaderProgram = 
                        CreateShaderPointer<ShaderProgram>(
                            std::vector<std::shared_ptr<Shader>>(
                            {
                                CreateShaderPointer<VertexShader>(std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.vert")),
                                CreateShaderPointer<FragmentShader>(std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.frag")),
                            }
                            )
                        );

                    m_RectTransform = new RectTransform("Transform");

                    m_RectTransform->awake();
                }

                void load_rect_transform()
                {
                }

                void setup_camera()
                {
                }
            };
        }
    }
};