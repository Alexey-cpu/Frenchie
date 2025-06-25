#pragma once

// Custom
#include <FrenchieRendererOpenGLShaderProgram.hpp>

#include <FrenchieCoreLogger.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// STL
#include <memory>

namespace Frenchie
{
    namespace Renderer
    {
        namespace OpenGL
        {
            struct Vertex
            {
                glm::vec3 Position;
                glm::vec3 Normal;
                glm::vec2 UV;
            };

            class Mesh
            {
            public:
                Mesh(){}
                
                virtual ~Mesh()
                {
                    glDeleteBuffers(1, &m_VBO);
                    glDeleteBuffers(1, &m_EBO);
                    glDeleteVertexArrays(1, &m_VAO);
                }

                bool is_instanced() const
                {
                    return !m_Vertexes.empty() && !m_Indexes.empty();
                }

                void setup() const
                {
                    if(!is_instanced()) 
                        return;

                    // Frenchie::Core::Logger::instance()->info("----------------------------------------------------------------");
                    // Frenchie::Core::Logger::instance()->info("Mesh::setup() operates !!!");
                    // Frenchie::Core::Logger::instance()->info(fmt::format("m_Vertexes.size() {}", m_Vertexes.size()));
                    // Frenchie::Core::Logger::instance()->info(fmt::format("m_Indexes.size() {}", m_Indexes.size()));
                    // Frenchie::Core::Logger::instance()->info("----------------------------------------------------------------");

                    // create buffers and vertex array
                    glGenBuffers(1, &m_VBO);
                    glGenBuffers(1, &m_EBO);
                    glGenVertexArrays(1, &m_VAO);

                    // bind VAO to remember VBO/EBO configuration and layout
                    glBindVertexArray(m_VAO);

                    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
                    glBufferData(GL_ARRAY_BUFFER, m_Vertexes.size() * sizeof(Vertex), &m_Vertexes[0], GL_DYNAMIC_DRAW);

                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indexes.size() * sizeof(int),  &m_Indexes[0], GL_DYNAMIC_DRAW);

                    // setup attributes pointers
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::Position)));

                    glEnableVertexAttribArray(1);
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::Normal)));

                    glEnableVertexAttribArray(2);
                    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::UV)));
                }

                void render() const
                {
                    if(!is_instanced()) 
                        return;

                    glBindVertexArray(m_VAO);
                    glDrawArrays(GL_POINTS, 0, (int)m_Indexes.size());
                    glDrawElements(GL_TRIANGLES, (int)m_Indexes.size(), GL_UNSIGNED_INT, 0);
                    glBindVertexArray(0);
                }

            protected:
                mutable unsigned int m_VBO  = 0;
                mutable unsigned int m_EBO  = 0;
                mutable unsigned int m_VAO  = 0;

                mutable std::vector<int>    m_Indexes  = std::vector<int>();
                mutable std::vector<Vertex> m_Vertexes = std::vector<Vertex>();
            };
        }
    }
}