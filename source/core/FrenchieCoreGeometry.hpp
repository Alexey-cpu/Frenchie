#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>

namespace Frenchie
{
    namespace Core
    {
        // Ray
        struct Ray final
        {
        public:
            
            Ray(const glm::vec3& _Origin    = glm::vec3(0.f), 
                const glm::vec3& _Direction = glm::vec3(0.f, 0.f, 1.f));
            
            ~Ray();

            // API
            Ray transform(const glm::mat4& _Transform) const;

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
                glm::vec3 _C = glm::vec3(0.f));

            ~Triangle();

            // API
            std::vector<glm::vec3> get_points() const;
            Triangle transform(const glm::mat4& _Transform) const;
            bool intersects(const Ray& _Ray) const;

            glm::vec3 A = glm::vec3(0.f);
            glm::vec3 B = glm::vec3(0.f);
            glm::vec3 C = glm::vec3(0.f);
        };

        // Aabb
        struct Aabb final
        {
        public:

            Aabb(const glm::vec3& _Min = glm::vec3(0.f), const glm::vec3& _Max = glm::vec3(16.f));

            ~Aabb();

            std::vector<glm::vec3> get_points() const;
            std::vector<Triangle> get_triangles() const;
            glm::vec3 get_size() const;
            glm::vec3 get_center() const;

            bool intersects(const Ray& _Ray) const;
            Aabb transform(const glm::mat4& _Transform) const;
            Aabb unite(const Aabb& _Other) const;
            Aabb grow(const glm::vec3& _Size) const;

            glm::vec3 Min = glm::vec3(0.f);
            glm::vec3 Max = glm::vec3(16.f);
        };
    }
}