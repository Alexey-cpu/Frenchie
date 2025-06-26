#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplication.hpp>

#include <FrenchieCoreFlyweight.hpp>
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreObject.hpp>

#include <FrenchieRendererCamera.hpp>
#include <FrenchieRendererViewport.hpp>
#include <FrenchieRendererScene.hpp>

#include <FrenchieRendererOpenGLMeshRenderer.hpp>
#include <FrenchieRendererOpenGLRectMesh.hpp>

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
            using namespace Frenchie::Renderer;
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
                
                virtual ~RenderingTest(){}

                virtual bool awake() override
                {
                    // create viewport
                    m_Viewport = create_shared_pointer<Viewport>(
                        100.f,
                        1.f,
                        90.f,
                        glm::vec3(1.f, 1.f, 1.f));

                    // create camera
                    create_raw_pointer<Camera>(
                        glm::vec3(+0.f, +0.f, +1.f),
                        glm::vec3(+0.f, +1.f, +0.f),
                        "Camera",
                        m_Viewport.get()
                    );

                    // create scene
                    Scene* scene = create_raw_pointer<Scene>(
                        "Scene",
                        m_Viewport.get()
                    );

                    // create shader
                    Shader* shader = 
                        FlyweightFactory::instance()->Create<Shader>(
                            std::vector<std::shared_ptr<ShaderLoader>>(
                            {
                                create_shared_pointer<ShaderLoader>(std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.vert")),
                                create_shared_pointer<ShaderLoader>(std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.frag")),
                            }
                        )
                    );

                    RectMesh* mesh = FlyweightFactory::instance()->Create<RectMesh>();

                    // create hierarchy
                    auto root    = new MeshRenderer(mesh, shader, "Root", scene);
                    auto child_1 = new MeshRenderer(mesh, shader, "Child-1", root);
                    auto child_2 = new MeshRenderer(mesh, shader, "Child-2", child_1);
                    auto child_3 = new MeshRenderer(mesh, shader, "Child-3", child_2);

                    root->set_position(glm::vec3(-0.1f, -0.1f, 0.f));
                    child_1->set_position(glm::vec3(0.1f, 0.1f, 0.f));
                    child_2->set_position(glm::vec3(0.1f, 0.1f, 0.f));
                    child_3->set_position(glm::vec3(0.1f, 0.1f, 0.f));

                    return m_Viewport->awake();
                }
                
                virtual void frame_start() override
                {
                    m_Viewport->frame_start();
                }
                
                virtual void frame_update() override
                {
                    m_Viewport->frame_update();
                }
                
                virtual void frame_finish() override
                {
                    m_Viewport->set_size(Frenchie::Application::GLApplication::instance()->get_window_size());

                    m_Viewport->frame_finish();

                    // // create world
                    // Viewport viewport = Viewport(
                    //     100.f,
                    //     1.f,
                    //     90.f,
                    //     glm::vec3(1.f, 1.f, 1.f)
                    // );

                    // // create camera
                    // Camera camera = Camera(
                    //     glm::vec3(+0.f, +0.f, +1.f),
                    //     glm::vec3(+0.f, +1.f, +0.f));

                    // glm::mat4 viewMatrix;
                    // glm::mat4 projectionMatrix;
                    // glm::vec3 viewportScale;

                    // // compute view matrix (camera matrix)
                    // {
                    //     viewMatrix = camera.get_view_matrix(viewport);
                    // }

                    // // compute projection matrix
                    // {
                    //     projectionMatrix = viewport.get_projection_matrix();
                    // }

                    // // compute world view port scale
                    // {
                    //     viewportScale = viewport.get_viewport_scale(Frenchie::Application::Application::instance()->get_window_size());
                    // }

                    // // draw here
                    // auto shader = FlyweightFactory::instance()->Request<ShaderProgram>();

                    // if(shader == nullptr) 
                    //     return;

                    // shader->begin();
                    // shader->set_uniform<glm::vec3>("u_ViewportScale", viewportScale);
                    // shader->set_uniform<glm::mat4>("u_ViewMatrix", viewMatrix);
                    // shader->set_uniform<glm::mat4>("u_ProjectionMatrix", projectionMatrix);

                    // m_Hierarchy->frame_finish();

                    // float angle = glm::degrees(2.f * glm::pi<float>() * 1.f * (float)glfwGetTime());

                    // m_Hierarchy->set_rotation(glm::vec3(angle, angle, 0.f));

                    // shader->end();
                }
                
                virtual void finish() override
                {
                }

            protected:

                std::shared_ptr<Viewport> m_Viewport = nullptr;
            };
        }
    }
};