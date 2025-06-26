#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplication.hpp>

#include <FrenchieCoreFlyweight.hpp>
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreObject.hpp>

#include <FrenchieRendererCamera.hpp>
#include <FrenchieRendererScene3D.hpp>

#include <FrenchieRendererOpenGLMeshRenderer.hpp>
#include <FrenchieRendererOpenGLMesh.hpp>

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
                    m_Viewport = create_shared_pointer<Scene3D>(
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

                    Rectangle2D* mesh = FlyweightFactory::instance()->Create<Rectangle2D>();

                    // create hierarchy
                    auto root    = new MeshRenderer(mesh, shader, "Root", m_Viewport.get());
                    auto child_1 = new MeshRenderer(mesh, shader, "Child-1", root);
                    auto child_2 = new MeshRenderer(mesh, shader, "Child-2", child_1);
                    auto child_3 = new MeshRenderer(mesh, shader, "Child-3", child_2);

                    root->set_position(glm::vec3(0.1f, 0.0f, 0.f));
                    root->set_rotation(glm::vec3(0.f, 0.f, 0.f));
                    
                    child_1->set_position(glm::vec3(200.f, 200.f, 0.f));
                    child_2->set_position(glm::vec3(200.f, 200.f, 0.f));
                    child_3->set_position(glm::vec3(200.f, 200.f, 0.f));

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

                    m_Viewport->apply_to_children(
                        [](Object* _Object)
                        {
                            Transform* transform = 
                                dynamic_cast<Transform*>(_Object);

                            if(transform == nullptr || dynamic_cast<Camera*>(_Object) != nullptr) 
                                return;

                            transform->set_rotation(
                                glm::vec3(
                                    0.f,//glm::degrees(2.f * glm::pi<float>() * 0.1f * (float)glfwGetTime()), 
                                    glm::degrees(2.f * glm::pi<float>() * 0.1f * (float)glfwGetTime()), 
                                    0.f//glm::degrees(2.f * glm::pi<float>() * 0.1f * (float)glfwGetTime())
                                )
                            );
                        }
                    );

                    m_Viewport->frame_finish();
                }
                
                virtual void finish() override
                {
                }

            protected:

                std::shared_ptr<Scene3D> m_Viewport = nullptr;
            };
        }
    }
};