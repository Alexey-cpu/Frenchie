#pragma once

// Core
#include <FrenchieCoreLogger.hpp>
#include <FrenchieCoreObject.hpp>

// Renderer
#include <FrenchieRendererTransformComponent.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>

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

        // Ray
        struct Ray final
        {
        public:
            
            Ray(const glm::vec3& _Origin = glm::vec3(0.f), const glm::vec3& _Direction = glm::vec3(0.f, 0.f, 1.f)) : 
                Origin(_Origin), 
                Direction(_Direction), 
                InverseDirection(1.f / Direction.x, 1.f / Direction.y, 1.f / Direction.z){}
            
            ~Ray(){}

            Ray transform(const glm::mat4& _Transform) const
            {
                return Ray(_Transform * glm::vec4(Origin, 1.f), glm::normalize(_Transform * glm::vec4(Direction, 1.f)));
            }

            glm::vec3 Origin           = glm::vec3(0.f, 0.f, 0.f);
            glm::vec3 Direction        = glm::vec3(0.f, 0.f, 1.f);
            glm::vec3 InverseDirection = glm::vec3(0.f, 0.f, 1.f);
        };

        // Triangle
        struct Triangle final
        {
        public:

            Triangle(glm::vec3 _P0 = glm::vec3(0.f), 
            glm::vec3 _P1 = glm::vec3(0.f), 
            glm::vec3 _P2 = glm::vec3(0.f)) : 
                P0(_P0), P1(_P1), P2(_P2){}

            glm::vec3 P0 = glm::vec3(0.f);
            glm::vec3 P1 = glm::vec3(0.f);
            glm::vec3 P2 = glm::vec3(0.f);
        };

        // Cube
        struct Cube final
        {
        public:

            Cube(glm::vec3 _Center = glm::vec3(0.f), float _Width = 100.f, float _Height = 100.f, float _Thickness = 100.f) : 
                Center(_Center), 
                Width(_Width), 
                Height(_Height), 
                Thickness(_Thickness){}

            ~Cube(){}

            std::vector<glm::vec3> get_points() const
            {
                return
                {
                    Center + glm::vec3(-Width, +Height, Thickness) * 0.5f,
                    Center + glm::vec3(+Width, +Height, Thickness) * 0.5f,
                    Center + glm::vec3(+Width, -Height, Thickness) * 0.5f,
                    Center + glm::vec3(-Width, -Height, Thickness) * 0.5f,
                    Center + glm::vec3(-Width, +Height, -Thickness) * 0.5f,
                    Center + glm::vec3(+Width, +Height, -Thickness) * 0.5f,
                    Center + glm::vec3(+Width, -Height, -Thickness) * 0.5f,
                    Center + glm::vec3(-Width, -Height, -Thickness) * 0.5f
                };
            }

            std::vector<Triangle> get_triangles() const
            {
                glm::vec3 A = Center + glm::vec3(-Width, +Height, Thickness) * 0.5f;
                glm::vec3 B = Center + glm::vec3(+Width, +Height, Thickness) * 0.5f;
                glm::vec3 C = Center + glm::vec3(+Width, -Height, Thickness) * 0.5f;
                glm::vec3 D = Center + glm::vec3(-Width, -Height, Thickness) * 0.5f;
                glm::vec3 E = Center + glm::vec3(-Width, +Height, -Thickness) * 0.5f;
                glm::vec3 F = Center + glm::vec3(+Width, +Height, -Thickness) * 0.5f;
                glm::vec3 G = Center + glm::vec3(+Width, -Height, -Thickness) * 0.5f;
                glm::vec3 H = Center + glm::vec3(-Width, -Height, -Thickness) * 0.5f;

                return
                {
                    // front
                    Triangle(A, B, D),
                    Triangle(D, B, C),

                    // back
                    Triangle(E, F, H),
                    Triangle(H, F, G),

                    // left
                    Triangle(A, E, D),
                    Triangle(E, H, D),

                    // right
                    Triangle(B, F, C),
                    Triangle(F, G, C),

                    // top
                    Triangle(A, E, F),
                    Triangle(F, B, A),

                    // bottom
                    Triangle(D, H, G),
                    Triangle(G, C, D),
                };
            }

            glm::vec3 Center    = glm::vec3(0.f); 
            float     Width     = 100.f; 
            float     Height    = 100.f; 
            float     Thickness = 100.f;
        };

        // Aabb
        struct Aabb final
        {
        public:

            Aabb(glm::vec3 _Min = glm::vec3(0.f), glm::vec3 _Max = glm::vec3(16.f)) : 
                Min(_Min), 
                Max(_Max){}

            ~Aabb(){}

            Aabb transform(const glm::mat4& _Transform) const
            {
                // transform all AABB points
                std::vector<glm::vec3> points = get_points();

                // generate new AABB
                glm::vec3 max = glm::vec3(-INFINITY);
                glm::vec3 min = glm::vec3(+INFINITY);

                for(auto&& point : points)
                {
                    point = _Transform * glm::vec4(point, 1.f);

                    max = glm::vec3(
                        std::max<float>(max.x, point.x), 
                        std::max<float>(max.y, point.y), 
                        std::max<float>(max.z, point.z));

                    min = glm::vec3(
                        std::min<float>(min.x, point.x), 
                        std::min<float>(min.y, point.y), 
                        std::min<float>(min.z, point.z));
                }

                return Aabb(min, max);
            }

            std::vector<glm::vec3> get_points() const
            {
                auto center = (Max + Min) * 0.5f;
                auto size   = (Max - Min);
                return Cube(
                    center, 
                    std::max<float>(size.x, 16.f), 
                    std::max<float>(size.y, 16.f), 
                    std::max<float>(size.z, 16.f)
                ).get_points();
            }

            std::vector<Triangle> get_triangles() const
            {
                auto center = (Max + Min) * 0.5f;
                auto size   = (Max - Min);
                return Cube(
                    center, 
                    std::max<float>(size.x, 16.f), 
                    std::max<float>(size.y, 16.f), 
                    std::max<float>(size.z, 16.f)
                ).get_triangles();
            }

            glm::vec3 Min = glm::vec3(0.f);
            glm::vec3 Max = glm::vec3(16.f);
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
    }
}