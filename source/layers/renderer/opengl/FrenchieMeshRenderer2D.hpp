#pragma once

// Custom
#include <FrenchieRendererMesh.hpp>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        // Mesh2D
        class Mesh2D : public Mesh
        {
        public:
            Mesh2D();
            virtual ~Mesh2D();
        };

        // Triangle2D
        class Triangle2D : public Mesh2D
        {
        public:
            Triangle2D();
            virtual ~Triangle2D();
        };

        // Rectangle2D
        class Rectangle2D : public Mesh2D
        {
        public:
            Rectangle2D();
            virtual ~Rectangle2D();
        };
    }
}