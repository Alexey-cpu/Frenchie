#include <FrenchieRendererOpenGLMeshRenderer.hpp>
#include <FrenchieCoreFlyweightFactory.hpp>

using namespace Frenchie::Renderer::OpenGL;

// Rect
Rect2D::Rect2D()
{
    m_Vertexes = 
    {
        // trangle 1
        { glm::vec3(-100.f, +100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
        { glm::vec3(+100.f, +100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
        { glm::vec3(-100.f, -100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },

        // trangle 2
        { glm::vec3(+100.f, +100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
        { glm::vec3(+100.f, -100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
        { glm::vec3(-100.f, -100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) }
    };

    m_Indexes = 
    {
        // triangle 1
        0, 1, 2,

        // triangle 2
        3, 4, 5
    };

    setup();
}

Rect2D::~Rect2D(){}