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

            class World final
            {
            public:

                World(float _Depth = 100.f, float _Aspect = 1.f, float _Fovy = 90.f, glm::vec3 _Axis = glm::vec3(1.f, 1.f, 1.f)) : 
                    m_Depth(_Depth), 
                    m_Aspect(_Aspect), 
                    m_Fovy(_Fovy),
                    m_Axis(_Axis){}

                ~World(){}

                glm::mat4 get_projection_matrix() const
                {
                    return glm::perspective(glm::radians(m_Fovy), m_Aspect, +0.1f, -m_Depth);
                }

                glm::vec3 get_viewport_scale(const glm::vec2& _WorldSize) const
                {
                    float scaleX = 1.f / std::max<float>((float)_WorldSize.x, 1.f);
                    float scaleY = 1.f / std::max<float>((float)_WorldSize.y, 1.f);
                    return glm::vec3(scaleX, scaleY, 1.f);
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

                    // info
                    float     m_Depth  = 100.f; 
                    float     m_Aspect = 1.f;
                    float     m_Fovy   = 90.f;
                    glm::vec3 m_Axis   = glm::vec3(1.f, 1.f, 1.f);
            };

            class Camera final
            {
            public:
                Camera(
                    glm::vec3 _CameraWorldPosition        = glm::vec3(+0.f, +0.f, +1.f), 
                    glm::vec3 _CameraWorldUpAxisDirection = glm::vec3(+0.f, +1.f, +0.f)) : 
                    m_CameraWorldPosition(_CameraWorldPosition), 
                    m_CameraWorldUpAxisDirection(_CameraWorldUpAxisDirection){}
                
                ~Camera(){}

                glm::vec3 get_position() const
                {
                    return m_CameraWorldPosition;
                }

                float get_pitch() const
                {
                    return m_Pitch;
                }

                float get_yaw() const
                {
                    return m_Yaw;
                }

                float get_roll() const
                {
                    return m_Roll;
                }
                
                void get_position(const glm::vec3& _Value)
                {
                    m_CameraWorldPosition = _Value;
                }

                void set_pitch(const float& _Value)
                {
                    m_Pitch = _Value;
                }

                void set_yaw(const float& _Value)
                {
                    m_Yaw = _Value;
                }

                void set_roll(const float& _Value)
                {
                    m_Roll = _Value;
                }
                
                glm::mat4 get_view_matrix(const World& _ViewPort) const
                {
                    // camera rotation angles
                    glm::mat4 rotateX  = glm::rotate(glm::mat4(1.f), glm::radians(m_Pitch), glm::vec3(1.f, 0.f, 0.f));
                    glm::mat4 rotateY  = glm::rotate(glm::mat4(1.f), glm::radians(m_Yaw), glm::vec3(0.f, 1.f, 0.f));
                    glm::mat4 rotateZ  = glm::rotate(glm::mat4(1.f), glm::radians(m_Roll), glm::vec3(0.f, 0.f, 1.f));

                    // camera local attributes
                    m_CameraLocalFrontAxisDirection = glm::vec3(0.f, 0.f, -_ViewPort.get_axis().z);
                    m_CameraLocalFrontAxisDirection = glm::normalize(rotateY * rotateX * glm::vec4(m_CameraLocalFrontAxisDirection, 1.f));
                    m_CameraLocalRightAxisDirection = glm::normalize(glm::cross(m_CameraLocalFrontAxisDirection, m_CameraWorldUpAxisDirection));
                    m_CameraLocalUpAxisDirection    = glm::normalize(glm::cross(m_CameraLocalRightAxisDirection, m_CameraLocalFrontAxisDirection));

                    m_CameraLocalFrontAxisDirection = glm::normalize(glm::vec3(rotateZ * glm::vec4(m_CameraLocalFrontAxisDirection, 1.f)));
                    m_CameraLocalUpAxisDirection    = glm::normalize(glm::vec3(rotateZ * glm::vec4(m_CameraLocalUpAxisDirection, 1.f)));

                    return glm::lookAt(m_CameraWorldPosition, m_CameraWorldPosition + m_CameraLocalFrontAxisDirection, m_CameraLocalUpAxisDirection);
                }
            
            protected:

                mutable glm::vec3 m_CameraWorldPosition           = glm::vec3(+0.f, +0.f, +1.f);
                mutable glm::vec3 m_CameraWorldUpAxisDirection    = glm::vec3(+0.f, +1.f, +0.f);
                mutable glm::vec3 m_CameraLocalFrontAxisDirection = glm::vec3(0.f);
                mutable glm::vec3 m_CameraLocalRightAxisDirection = glm::vec3(0.f);
                mutable glm::vec3 m_CameraLocalUpAxisDirection    = glm::vec3(0.f);
                mutable float     m_Pitch                         = 0.f;
                mutable float     m_Yaw                           = 0.f;
                mutable float     m_Roll                          = 0.f;
                
            };

            class Model
            {
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
                    // create world
                    World viewport = World(
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