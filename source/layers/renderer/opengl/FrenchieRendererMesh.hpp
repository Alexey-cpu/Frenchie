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
#include <iostream>

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
            
            Ray(const glm::vec3& _Origin    = glm::vec3(0.f), 
                const glm::vec3& _Direction = glm::vec3(0.f, 0.f, 1.f)) : 
                Origin(_Origin), 
                Direction(_Direction), 
                InverseDirection(1.f / Direction.x, 1.f / Direction.y, 1.f / Direction.z){}
            
            ~Ray(){}

            // API
            Ray transform(const glm::mat4& _Transform) const
            {
                return Ray(
                    _Transform * glm::vec4(Origin, 1.f), 
                    glm::normalize(_Transform * glm::vec4(Direction, 1.f)));
            }

            glm::vec3 Origin           = glm::vec3(0.f, 0.f, 0.f);
            glm::vec3 Direction        = glm::vec3(0.f, 0.f, 1.f);
            glm::vec3 InverseDirection = glm::vec3(0.f, 0.f, 1.f);
        };

        // Triangle
        struct Triangle final
        {
        public:

            Triangle(
                glm::vec3 _A = glm::vec3(0.f), 
                glm::vec3 _B = glm::vec3(0.f),  
                glm::vec3 _C = glm::vec3(0.f)) : 
            A(_A), B(_B), C(_C){}

            // API
            std::vector<glm::vec3> get_points() const
            {
                return {A, B, C};
            }

            Triangle transform(const glm::mat4& _Transform) const
            {
                return Triangle(
                    _Transform * glm::vec4(A, 1.f), 
                    _Transform * glm::vec4(B, 1.f), 
                    _Transform * glm::vec4(C, 1.f)
                );
            }

            bool intersects(Ray _Ray) const
            {
                // retrieve ray origin point and direction vector
                auto O = _Ray.Origin;
                auto R = _Ray.Direction;

                // get trianle plain's normal
                auto BA = B - A;
                auto AC = A - C;
                auto CB = C - B;
                auto N  = glm::normalize(glm::cross(BA, AC));
                
                // check if triangle and ray are parallel
                // (triangle and ray are parallel if the trianle normal is perpendicular to the ray)
                if(std::abs(glm::dot(N, R)) < 4.f * std::numeric_limits<float>::min()) 
                    return false;

                // find the point where the ray hits trianle plain
                auto  D = -glm::dot(N, A);
                float t = -(glm::dot(N, O) + D) / dot(N, R);
                auto  P = O + t * R;

                // Check if the triangle is behind the ray
                if (t < 0) 
                    return false;

                // check if the hit point belongs to triangle
                if((glm::dot(N, glm::cross(AC, (C - P)))) < 0.f) 
                    return false;

                if((glm::dot(N, glm::cross((P - B), CB))) < 0.f)
                    return false;

                if((glm::dot(N, glm::cross((P - A), BA))) < 0.f) 
                    return false;

                return true;
            }

            glm::vec3 A = glm::vec3(0.f);
            glm::vec3 B = glm::vec3(0.f);
            glm::vec3 C = glm::vec3(0.f);
        };

        // Aabb
        struct Aabb final
        {
        public:

            Aabb(
                const glm::vec3& _Min = glm::vec3(0.f), 
                const glm::vec3& _Max = glm::vec3(16.f)) : 
                Min(_Min), 
                Max(_Max){}

            ~Aabb(){}

            Aabb transform(const glm::mat4& _Transform) const
            {
                std::vector<glm::vec3> points = get_points();
                glm::vec3              max    = glm::vec3(-INFINITY);
                glm::vec3              min    = glm::vec3(+INFINITY);

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

            bool intersects(Ray _Ray) const
            {
                double tmin = -INFINITY;
                double tmax = +INFINITY;

                for (int i = 0; i < 3; ++i) 
                {
                    if (_Ray.Direction[i] != 0.0) 
                    {
                        double t1 = (Min[i] - _Ray.Origin[i]) / _Ray.Direction[i];
                        double t2 = (Max[i] - _Ray.Origin[i]) / _Ray.Direction[i];

                        tmin = std::max(tmin, std::min(t1, t2));
                        tmax = std::min(tmax, std::max(t1, t2));
                    } 
                    else if (_Ray.Origin[i] <= Min[i] || _Ray.Origin[i] >= Max[i]) 
                    {
                        return false;
                    }
                }

                std::cout << "tmax " << tmax << " tmin " << tmin << "\n";

                return tmax > tmin;
            }

            glm::vec3 Min = glm::vec3(0.f);
            glm::vec3 Max = glm::vec3(16.f);

        private:

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
                    glm::vec3 P0 = Center + glm::vec3(-Width, +Height, 0.f) * 0.5f;
                    glm::vec3 P1 = Center + glm::vec3(+Width, +Height, 0.f) * 0.5f;
                    glm::vec3 P2 = Center + glm::vec3(+Width, -Height, 0.f) * 0.5f;
                    glm::vec3 P3 = Center + glm::vec3(-Width, -Height, 0.f) * 0.5f;

                    return
                    {
                        glm::vec3(P0.x, P0.y, +Thickness * 0.5f),
                        glm::vec3(P1.x, P1.y, +Thickness * 0.5f),
                        glm::vec3(P2.x, P2.y, +Thickness * 0.5f),
                        glm::vec3(P3.x, P3.y, +Thickness * 0.5f),
                        glm::vec3(P0.x, P0.y, -Thickness * 0.5f),
                        glm::vec3(P1.x, P1.y, -Thickness * 0.5f),
                        glm::vec3(P2.x, P2.y, -Thickness * 0.5f),
                        glm::vec3(P3.x, P3.y, -Thickness * 0.5f),
                    };
                }

                std::vector<Triangle> get_triangles() const
                {
                    glm::vec3 P0 = Center + glm::vec3(-Width, +Height, 0.f) * 0.5f;
                    glm::vec3 P1 = Center + glm::vec3(+Width, +Height, 0.f) * 0.5f;
                    glm::vec3 P2 = Center + glm::vec3(+Width, -Height, 0.f) * 0.5f;
                    glm::vec3 P3 = Center + glm::vec3(-Width, -Height, 0.f) * 0.5f;

                    glm::vec3 A = glm::vec3(P0.x, P0.y, +Thickness * 0.5f);
                    glm::vec3 B = glm::vec3(P1.x, P1.y, +Thickness * 0.5f);
                    glm::vec3 C = glm::vec3(P2.x, P2.y, +Thickness * 0.5f);
                    glm::vec3 D = glm::vec3(P3.x, P3.y, +Thickness * 0.5f);
                    glm::vec3 E = glm::vec3(P0.x, P0.y, -Thickness * 0.5f);
                    glm::vec3 F = glm::vec3(P1.x, P1.y, -Thickness * 0.5f);
                    glm::vec3 G = glm::vec3(P2.x, P2.y, -Thickness * 0.5f);
                    glm::vec3 H = glm::vec3(P3.x, P3.y, -Thickness * 0.5f);

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