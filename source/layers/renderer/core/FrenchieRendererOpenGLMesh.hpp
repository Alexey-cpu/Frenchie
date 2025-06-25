#pragma once

// Custom
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
            using namespace Core;

            struct Vertex
            {
                glm::vec3 Position;
                glm::vec3 Normal;
                glm::vec2 UV;
            };

            class Mesh
            {
            public:
                Mesh();
                virtual ~Mesh();

                bool is_instanced() const;
                void setup();
                void render();

            protected:
                mutable unsigned int        m_VBO      = 0;
                mutable unsigned int        m_EBO      = 0;
                mutable unsigned int        m_VAO      = 0;
                mutable std::vector<int>    m_Indexes  = std::vector<int>();
                mutable std::vector<Vertex> m_Vertexes = std::vector<Vertex>();
            };
        }
    }
}