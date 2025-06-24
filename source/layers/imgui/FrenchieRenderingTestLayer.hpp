#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieRendererOpenGLShaderProgram.hpp>
#include <FrenchieRendererOpenGLCamera.hpp>
#include <FrenchieApplication.hpp>

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
            using namespace Frenchie::Renderer::OpenGL;
            using namespace Frenchie::Core;

            class ViewPort
            {
                public:

                ViewPort(float _Depth = 100.f, float _Aspect = 1.f, float _Fovy = 90.f, glm::vec3 _Axis = glm::vec3(1.f, 1.f, 1.f)) : 
                    m_Depth(_Depth), 
                    m_Aspect(_Aspect), 
                    m_Fovy(_Fovy),
                    m_Axis(_Axis){}

                virtual ~ViewPort(){}

                glm::mat4 get_projection_matrix() const
                {
                    return glm::perspective(glm::radians(m_Fovy), m_Aspect, +0.1f, -m_Depth);
                }

                virtual glm::vec3 get_viewport_scale() const
                {
                    auto size = glm::vec2(Frenchie::Application::Application::instance()->get_window_size());

                    return glm::vec3(1.f / (float)size.x, 1.f / (float)size.y, 1.f);
                }

                glm::vec3 get_axis() const
                {
                    return m_Axis;
                }

                float get_aspect() const
                {
                    return m_Aspect;
                }

                float get_depth() const
                {
                    return m_Depth;
                }

                float get_fovy() const
                {
                    return m_Fovy;
                }

                void set_axis(const glm::vec3& _Value)
                {
                    m_Axis = _Value;
                }

                void set_aspect(const float& _Value)
                {
                    m_Aspect = _Value;
                }

                void set_depth(const float& _Value)
                {
                    m_Depth = _Value;
                }

                void set_fovy(const float& _Value)
                {
                    m_Fovy = _Value;
                }

                protected:

                    float     m_Depth  = 100.f; 
                    float     m_Aspect = 1.f;
                    float     m_Fovy   = 90.f;
                    glm::vec3 m_Axis   = glm::vec3(1.f, 1.f, 1.f);
            };

            struct Vertex
            {
                glm::vec3 Position;
                glm::vec4 Color;
            };

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

                    ViewPort viewport = ViewPort(
                        100.f,
                        1.f,
                        90.f,
                        glm::vec3(1.f, 1.f, 1.f)
                    );

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
                        float velocity = 2.f * glm::pi<float>() * 0.1f * (float)glfwGetTime();
                        float pitch    = 0.f;
                        float yaw      = 0.f;
                        float roll     = 0.f;

                        // camera world attributes
                        glm::vec3 cameraWorldPosition        = glm::vec3(+0.f, +0.f, +1.f);
                        glm::vec3 cameraWorldUpAxisDirection = glm::vec3(+0.f, +1.f, +0.f);

                        // camera rotations
                        glm::mat4 rotateX  = glm::rotate(glm::mat4(1.f), glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f));
                        glm::mat4 rotateY  = glm::rotate(glm::mat4(1.f), glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f));
                        glm::mat4 rotateZ  = glm::rotate(glm::mat4(1.f), glm::radians(roll), glm::vec3(0.f, 0.f, 1.f));

                        // camera local attributes
                        glm::vec3 cameraLocalFrontAxisDirection = glm::vec3(0.f, 0.f, -viewport.get_axis().z);
                        cameraLocalFrontAxisDirection           = glm::normalize(rotateY * rotateX * glm::vec4(cameraLocalFrontAxisDirection, 1.f));
                        glm::vec3 cameraLocalRightAxisDirection = glm::normalize(glm::cross(cameraLocalFrontAxisDirection, cameraWorldUpAxisDirection));
                        glm::vec3 cameraLocalUpAxisDirection    = glm::normalize(glm::cross(cameraLocalRightAxisDirection, cameraLocalFrontAxisDirection));

                        cameraLocalFrontAxisDirection = glm::normalize(glm::vec3(rotateZ * glm::vec4(cameraLocalFrontAxisDirection, 1.f)));
                        cameraLocalUpAxisDirection    = glm::normalize(glm::vec3(rotateZ * glm::vec4(cameraLocalUpAxisDirection, 1.f)));

                        viewMatrix = glm::lookAt(cameraWorldPosition, cameraWorldPosition + cameraLocalFrontAxisDirection, cameraLocalUpAxisDirection);
                    }

                    // compute projection matrix
                    {
                        projectionMatrix = viewport.get_projection_matrix();
                        viewportScale    = viewport.get_viewport_scale();
                    }

                    // draw here
                    m_ShaderProgram->use();
                    m_ShaderProgram->set_uniform<glm::vec3>("u_Scale", viewportScale);
                    m_ShaderProgram->set_uniform<glm::mat4>("u_Model", modelMatrix);
                    m_ShaderProgram->set_uniform<glm::mat4>("u_View", viewMatrix);
                    m_ShaderProgram->set_uniform<glm::mat4>("u_Projection", projectionMatrix);

                    // draw
                    glBindVertexArray(m_VAO);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw the triangle

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