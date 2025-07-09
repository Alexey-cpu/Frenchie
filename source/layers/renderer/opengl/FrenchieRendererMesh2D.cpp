#include <FrenchieRendererMesh2D.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Renderer;

#include <iostream>

// Triangle2D
Triangle2D::Triangle2D() : 
Mesh(std::vector<Vertex>({
    // trangle 1
    { glm::vec3(-100.f, +100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
    { glm::vec3(+100.f, +100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
    { glm::vec3(-100.f, -100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) }
})){}

Triangle2D::~Triangle2D(){}

// Rectangle2D
Rectangle2D::Rectangle2D() : 
Mesh(std::vector<Vertex>({
    // trangle 1
    { glm::vec3(-100.f, +100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
    { glm::vec3(+100.f, +100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
    { glm::vec3(-100.f, -100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },

    // trangle 2
    { glm::vec3(+100.f, +100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
    { glm::vec3(+100.f, -100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
    { glm::vec3(-100.f, -100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) }
})){}

Rectangle2D::~Rectangle2D(){}

// Rectangle3D
Rectangle3D::Rectangle3D() : 
Mesh(Rectangle3D::generateVertexes()){}

Rectangle3D::~Rectangle3D(){}

std::vector<Vertex> Rectangle3D::generateVertexes()
{
    std::vector<Vertex> vertexes;

    auto triangles = Aabb(
        glm::vec3(-100.f, -100.f, -100.f), 
        glm::vec3(+100.f, +100.f, +100.f)).get_triangles();

    for(auto&& triangle : triangles)
    {
        vertexes.push_back({triangle.A, glm::vec3(0.f), glm::vec2(0.f)});
        vertexes.push_back({triangle.B, glm::vec3(0.f), glm::vec2(0.f)});
        vertexes.push_back({triangle.C, glm::vec3(0.f), glm::vec2(0.f)});
    }

    return vertexes;
}