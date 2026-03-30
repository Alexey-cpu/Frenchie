#pragma once

// Core
#include <FrenchieCoreLogger.hpp>
#include <FrenchieCoreObject.hpp>
#include <FrenchieCoreGeometry.hpp>

// Renderer
#include <FrenchieRendererTransformComponent.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

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
            Mesh(std::vector<Vertex> _Vertexes);
            virtual ~Mesh();

            // getters
            Aabb get_aabb() const;

            // API
            bool instantiate();
            void render();

        protected:

            // info
            mutable unsigned int        m_VBO      = 0;
            mutable unsigned int        m_EBO      = 0;
            mutable unsigned int        m_VAO      = 0;
            mutable std::vector<int>    m_Indexes  = std::vector<int>();
            mutable std::vector<Vertex> m_Vertexes = std::vector<Vertex>();
            mutable Aabb                m_AABB     = Aabb(glm::vec3(0.f, 0.f, 0.f), glm::vec3(16.f, 16.f, 16.f));
        };

        class IMesh
        {
        public:
            IMesh(){}
            virtual ~IMesh(){}

            virtual std::shared_ptr<Mesh> get_mesh() const = 0;
        };
    }
}