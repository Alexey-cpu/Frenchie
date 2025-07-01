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

            virtual bool contains(glm::vec3 _Point) const 
            {
                (void)_Point;
                return false;
            }

        protected:
            mutable unsigned int        m_VBO      = 0;
            mutable unsigned int        m_EBO      = 0;
            mutable unsigned int        m_VAO      = 0;
            mutable std::vector<int>    m_Indexes  = std::vector<int>();
            mutable std::vector<Vertex> m_Vertexes = std::vector<Vertex>();

            bool is_instanced() const;
        };

        // int pnpoly(int npol, float * xp, float * yp, float x, float y)
        // {
        //     int c = 0;
        //     for (int i = 0, j = npol - 1; i < npol; j = i++) 
        //     {
        //     if ((
        //         (yp[i] < yp[j]) && (yp[i] <= y) && (y <= yp[j]) &&
        //         ((yp[j] - yp[i]) * (x - xp[i]) > (xp[j] - xp[i]) * (y - yp[i]))
        //     ) || (
        //         (yp[i] > yp[j]) && (yp[j] <= y) && (y <= yp[i]) &&
        //         ((yp[j] - yp[i]) * (x - xp[i]) < (xp[j] - xp[i]) * (y - yp[i]))
        //     ))
        //         c = !c;
        //     }
        //     return c;
        // }

        class Polygon2D : public Mesh
        {
        public:
            Polygon2D(){}
            virtual ~Polygon2D(){}

            virtual bool contains(glm::vec3 _Point) const override 
            {
                return is_inside(m_Vertexes, _Point);
            }

        protected:

            bool is_inside(std::vector<Vertex> _Poly, glm::vec3 _Point) const
            {
                int c = 0;
                int npol = (int)_Poly.size();

                for (int i = 0, j = npol - 1; i < npol; j = i++) 
                {
                    if ((
                    (_Poly[i].Position.y < _Poly[j].Position.y) && (_Poly[i].Position.y <= _Point.y) && (_Point.y <= _Poly[j].Position.y) &&
                    ((_Poly[j].Position.y - _Poly[i].Position.y) * (_Point.x - _Poly[i].Position.x) > (_Poly[j].Position.x - _Poly[i].Position.x) * (_Point.y - _Poly[i].Position.y))
                    ) || (
                    (_Poly[i].Position.y > _Poly[j].Position.y) && (_Poly[j].Position.y <= _Point.y) && (_Point.y <= _Poly[i].Position.y) &&
                    ((_Poly[j].Position.y - _Poly[i].Position.y) * (_Point.x - _Poly[i].Position.x) < (_Poly[j].Position.x - _Poly[i].Position.x) * (_Point.y - _Poly[i].Position.y))
                    ))
                    {
                        c = !c;
                    }
                }
                return c;
            }
        };

        // Triangle2D
        class Triangle2D : public Polygon2D
        {
        public:
            Triangle2D();
            virtual ~Triangle2D();
        };

        // Rectangle2D
        class Rectangle2D : public Polygon2D
        {
        public:
            Rectangle2D();
            virtual ~Rectangle2D();
        };
    }
}