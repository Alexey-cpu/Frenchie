#pragma once

// Core
#include <FrenchieCoreLogger.hpp>
#include <FrenchieCoreObject.hpp>

// Renderer
#include <FrenchieRendererShader.hpp>
#include <FrenchieRendererTransform.hpp>

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
        // Vertex
        struct Vertex
        {
            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec2 UV;
        };

        // Mesh
        class Mesh
        {
        public:
            Mesh();
            virtual ~Mesh();

            // API
            bool instantiate();
            void render();

        protected:
            mutable unsigned int        m_VBO      = 0;
            mutable unsigned int        m_EBO      = 0;
            mutable unsigned int        m_VAO      = 0;
            mutable std::vector<int>    m_Indexes  = std::vector<int>();
            mutable std::vector<Vertex> m_Vertexes = std::vector<Vertex>();

            bool is_instanced() const;
        };

        // MeshRenderer
        class MeshRenderer : public Transform
        {
        public:
            MeshRenderer(
                Mesh*              _Mesh,
                Shader*            _Shader,
                const std::string& _Name = std::string());
            
            virtual ~MeshRenderer();

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_finish() override;

        protected:

            Mesh*   m_Mesh   = nullptr;
            Shader* m_Shader = nullptr;
        };

        // Triangle2D
        class Triangle2D : public Mesh
        {
        public:
            Triangle2D();
            virtual ~Triangle2D();
        };

        // Rectangle2D
        class Rectangle2D : public Mesh
        {
        public:
            Rectangle2D();
            virtual ~Rectangle2D();
        };
    }
}