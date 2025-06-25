#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplication.hpp>

#include <FrenchieCoreFlyweightFactory.hpp>
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreObject.hpp>

#include <FrenchieRendererOpenGLCamera.hpp>
#include <FrenchieRendererOpenGLShaderProgram.hpp>
#include <FrenchieRendererOpenGLMeshRenderer.hpp>

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
                    m_Hierarchy->frame_start();
                }
                
                virtual void frame_update() override
                {
                    m_Hierarchy->frame_update();
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

                    glm::mat4 viewMatrix;
                    glm::mat4 projectionMatrix;
                    glm::vec3 viewportScale;

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
                    auto shader = FlyweightFactory::instance()->Request<ShaderProgram>();

                    if(shader == nullptr) 
                        return;

                    shader->begin();
                    shader->set_uniform<glm::vec3>("u_ViewportScale", viewportScale);
                    shader->set_uniform<glm::mat4>("u_ViewMatrix", viewMatrix);
                    shader->set_uniform<glm::mat4>("u_ProjectionMatrix", projectionMatrix);

                    m_Hierarchy->frame_finish();

                    float angle = glm::degrees(2.f * glm::pi<float>() * 1.f * (float)glfwGetTime());

                    m_Hierarchy->set_rotation(glm::vec3(angle, angle, 0.f));

                    shader->end();
                }
                
                virtual void finish() override
                {
                }

            protected:

                MeshRenderer* m_Hierarchy = nullptr;

                void setup_mesh(){}

                void load_shaders()
                {
                    ShaderProgram* shader = 
                        FlyweightFactory::instance()->Create<ShaderProgram>(
                            std::vector<std::shared_ptr<Shader>>(
                            {
                                CreateSharedPointer<VertexShader>(std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.vert")),
                                CreateSharedPointer<FragmentShader>(std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.frag")),
                            }
                        )
                    );

                    Rect2D* mesh = FlyweightFactory::instance()->Create<Rect2D>();

                    // create hierarchy
                    auto root    = new MeshRenderer(mesh, shader, "Root");
                    auto child_1 = new MeshRenderer(mesh, shader, "Child-1", root);
                    auto child_2 = new MeshRenderer(mesh, shader, "Child-2", child_1);
                    auto child_3 = new MeshRenderer(mesh, shader, "Child-3", child_2);

                    root->set_position(glm::vec3(-0.1f, -0.1f, 0.f));
                    child_1->set_position(glm::vec3(0.1f, 0.1f, 0.f));
                    child_2->set_position(glm::vec3(0.1f, 0.1f, 0.f));
                    child_3->set_position(glm::vec3(0.1f, 0.1f, 0.f));
                    
                    m_Hierarchy = root;

                    Frenchie::Core::Logger::instance()->info("Hierarchy");
                    m_Hierarchy->apply_to_children_recursive([](Object* _Object)
                    {
                        Frenchie::Core::Logger::instance()->info(fmt::format("{}", _Object->get_name()));
                    });

                    m_Hierarchy->awake();
                }
            };
        }
    }
};