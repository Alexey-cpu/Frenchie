#pragma once

// Custom
#include <FrenchieCoreLogger.hpp>
#include <FrenchieCoreObject.hpp>

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
        struct Vertex
        {
            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec2 UV;
        };

        namespace OpenGL
        {
            class Mesh : public Frenchie::Core::IRenderer
            {
            public:
                Mesh();
                virtual ~Mesh();

                // virtual API
                virtual bool awake()        override;
                virtual void frame_start()  override;
                virtual void frame_update() override;
                virtual void frame_finish() override;

            protected:
                mutable unsigned int        m_VBO      = 0;
                mutable unsigned int        m_EBO      = 0;
                mutable unsigned int        m_VAO      = 0;
                mutable std::vector<int>    m_Indexes  = std::vector<int>();
                mutable std::vector<Vertex> m_Vertexes = std::vector<Vertex>();

                bool is_instanced() const;
            };

            class Rectangle2D : public Mesh
            {
            public:
                Rectangle2D();
                virtual ~Rectangle2D();
            };
        }
    }
}