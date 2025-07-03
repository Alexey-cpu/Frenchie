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