#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplication.hpp>

#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreObject.hpp>

#include <FrenchieRendererOpenGLCamera.hpp>
#include <FrenchieRendererOpenGLShaderProgram.hpp>

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
                glm::vec4 Color;
            };

            // class Mesh : public Frenchie::Core::Object, public IDrawable
            // {
            // public:
                
            //     Mesh(std::string _Name = std::string(), Object* _Parent = nullptr) : 
            //         Frenchie::Core::Object(_Name, _Parent){}

            //     virtual ~Mesh()
            //     {
            //         glDeleteVertexArrays(1, &m_VAO);
            //         glDeleteBuffers(1, &m_VBO);
            //         glDeleteBuffers(1, &m_EBO);
            //     }

            //     bool awake() override
            //     {
            //         // create buffers and vertex array
            //         glGenBuffers(1, &m_VBO);
            //         glGenBuffers(1, &m_EBO);
            //         glGenVertexArrays(1, &m_VAO);

            //         // load data into VBO and EBO
            //         glBindVertexArray(m_VAO);
            //         glBufferData(m_VBO, m_Vertexes.size() * sizeof(Vertex), &m_Vertexes[0], GL_DYNAMIC_DRAW);
            //         glBufferData(m_EBO, m_Indexes.size() * sizeof(int), &m_Indexes[0], GL_DYNAMIC_DRAW);

            //         // setup attributes pointers
            //         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::Position)));
            //         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::Normal)));
            //         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::UV)));
            //         glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::Color)));
            //         glEnableVertexAttribArray(0);
            //         glEnableVertexAttribArray(1);
            //         glEnableVertexAttribArray(2);
            //         glEnableVertexAttribArray(3);

            //         return true;
            //     }
                
            //     virtual void frame_start()  override{}
                
            //     virtual void frame_update() override{}
                
            //     virtual void frame_finish() override
            //     {
            //         glBindVertexArray(m_VAO);
            //         glDrawArrays(GL_POINTS, 0, 6);
            //         glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //     }

            //     void updateSelfAndChild()
            //     {
            //         if (get_parent<Mesh>() != nullptr)
            //             m_ModelMatrix = get_parent<Mesh>()->m_ModelMatrix * m_Transform.get_model_matrix();
            //         else
            //             m_ModelMatrix = m_Transform.get_model_matrix();

            //         for (auto&& child : m_Children)
            //         {
            //             Mesh* object = dynamic_cast<Mesh*>(child);

            //             if(object != nullptr)
            //                 object->updateSelfAndChild();
            //         }
            //     }
            
            // protected:

            //     unsigned int                   m_VAO           = 0;    
            //     unsigned int                   m_VBO           = 0;
            //     unsigned int                   m_EBO           = 0;
            //     std::vector<Vertex>            m_Vertexes      = std::vector<Vertex>();
            //     std::vector<int>               m_Indexes       = std::vector<int>();
            //     std::shared_ptr<ShaderProgram> m_ShaderProgram = nullptr;

            //     Transform m_Transform   = Transform();
            //     glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
            // };

            class RenderingTest : public Layer
            {
            public:

                RenderingTest(const std::string& _Name = "RenderingTest") : Layer(_Name){}
                
                virtual ~RenderingTest()
                {
                    glDeleteBuffers(1, &m_VBO);
                    glDeleteBuffers(1, &m_EBO);
                    glDeleteVertexArrays(1, &m_VAO);
                }

                virtual bool awake() override
                {
                    setup_mesh();
                    load_shaders();
                    return true;
                }
                
                virtual void frame_start() override
                {
                }
                
                virtual void frame_update() override
                {
                    // compute geometry here
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
                        modelMatrix = glm::rotate(glm::mat4(1.0f), 0.f, glm::vec3(1.0f, 0.0f, 0.0f));
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
                    m_ShaderProgram->use();
                    m_ShaderProgram->set_uniform<glm::vec3>("u_Scale", viewportScale);
                    m_ShaderProgram->set_uniform<glm::mat4>("u_Model", modelMatrix);
                    m_ShaderProgram->set_uniform<glm::mat4>("u_View", viewMatrix);
                    m_ShaderProgram->set_uniform<glm::mat4>("u_Projection", projectionMatrix);

                    // draw
                    glBindVertexArray(m_VAO);
                    glDrawArrays(GL_POINTS, 0, 6);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw the triangle

                    //glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 100);

                    m_ShaderProgram->unuse();
                }
                
                virtual void finish() override
                {
                }

            protected:
                unsigned int                   m_VAO           = 0;    
                unsigned int                   m_VBO           = 0;
                unsigned int                   m_EBO           = 0;
                std::vector<Vertex>            m_Vertexes      = std::vector<Vertex>();
                std::vector<int>               m_Indexes       = std::vector<int>();
                std::shared_ptr<ShaderProgram> m_ShaderProgram = nullptr;

                void setup_mesh()
                {
                    // data
                    // m_Vertexes = 
                    // {
                    //     // trangle 1
                    //     { glm::vec3(-0.5f, +0.5f, 0.f), glm::vec4(1.f, 1.f, 1.f, 1.f) },
                    //     { glm::vec3(+0.5f, +0.5f, 0.f), glm::vec4(1.f, 1.f, 1.f, 1.f) },
                    //     { glm::vec3(-0.5f, -0.5f, 0.f), glm::vec4(1.f, 1.f, 1.f, 1.f) },

                    //     // trangle 2
                    //     { glm::vec3(+0.5f, +0.5f, 0.f), glm::vec4(1.f, 1.f, 1.f, 1.f) },
                    //     { glm::vec3(+0.5f, -0.5f, 0.f), glm::vec4(1.f, 1.f, 1.f, 1.f) },
                    //     { glm::vec3(-0.5f, -0.5f, 0.f), glm::vec4(1.f, 1.f, 1.f, 1.f) }
                    // };

                    m_Vertexes = 
                    {
                        // trangle 1
                        { glm::vec3(-200.f, +200.f, 0.f), glm::vec4(1.f, 1.f, 1.f, 0.5f) },
                        { glm::vec3(+200.f, +200.f, 0.f), glm::vec4(1.f, 1.f, 1.f, 0.5f) },
                        { glm::vec3(-200.f, -200.f, 0.f), glm::vec4(1.f, 1.f, 1.f, 0.5f) },

                        // trangle 2
                        { glm::vec3(+200.f, +200.f, 0.f), glm::vec4(1.f, 1.f, 1.f, 0.5f) },
                        { glm::vec3(+200.f, -200.f, 0.f), glm::vec4(1.f, 1.f, 1.f, 0.5f) },
                        { glm::vec3(-200.f, -200.f, 0.f), glm::vec4(1.f, 1.f, 1.f, 0.5f) }
                    };

                    m_Indexes = 
                    {
                        // triangle 1
                        0, 1, 2,

                        // triangle 2
                        3, 4, 5
                    };

                    glEnable(GL_PROGRAM_POINT_SIZE);

                    // generate buffers
                    glGenVertexArrays(1, &m_VAO);
                    glGenBuffers(1, &m_VBO);
                    glGenBuffers(1, &m_EBO);

                    // bind vertex array
                    glBindVertexArray(m_VAO);

                    // load data to VBO
                    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
                    glBufferData(GL_ARRAY_BUFFER, m_Vertexes.size() * sizeof(Vertex), &m_Vertexes[0], GL_STATIC_DRAW);

                    // load data to EBO
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indexes.size() * sizeof(unsigned int),  &m_Indexes[0], GL_STATIC_DRAW);

                    // setup vertex attributes
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::Position)));

                    glEnableVertexAttribArray(1);
                    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::Color)));
                }

                void load_shaders()
                {
                    m_ShaderProgram = 
                        CreateShaderPointer<ShaderProgram>(
                            std::vector<std::shared_ptr<Shader>>(
                            {
                                CreateShaderPointer<VertexShader>(std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/shaders/Simple/Simple.vert")),
                                CreateShaderPointer<FragmentShader>(std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/shaders/Simple/Simple.frag")),
                            }
                            )
                        );
                }

                void setup_camera()
                {
                }
            };
        }
    }
};