#include <FrenchieRendererOpenGLRectTransform.hpp>
#include <FrenchieCoreFlyweightFactory.hpp>

using namespace Frenchie::Renderer::OpenGL;

// Rect
Rect::Rect()
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

Rect::~Rect(){}

// RectTransform
RectTransform::RectTransform(const std::string& _Name, Object* _Parent) : 
    Transform(_Name, _Parent){}

RectTransform::~RectTransform(){}

bool RectTransform::awake()
{
    if(!Transform::awake()) 
        return false;

    // instantiate mesh
    m_Mesh = Core::FlyweightFactory::instance()->Create<Rect>();

    return m_Mesh != nullptr;
}

void RectTransform::frame_start()
{
    Transform::frame_start();
}

void RectTransform::frame_update()
{
    Transform::frame_update();
}

void RectTransform::frame_finish()
{
    // draw mesh
    if(m_Mesh != nullptr) 
        m_Mesh->render();
}