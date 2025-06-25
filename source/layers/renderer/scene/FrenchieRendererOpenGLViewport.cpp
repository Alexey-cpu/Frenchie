#include <FrenchieRendererOpenGLViewport.hpp>

using namespace Frenchie::Renderer::OpenGL;

Viewport::Viewport(float _Depth, float _Aspect, float _Fovy, glm::vec3 _Axis) : 
    m_Depth(_Depth), m_Aspect(_Aspect), m_Fovy(_Fovy), m_Axis(_Axis){}

Viewport::~Viewport(){}

glm::mat4 Viewport::get_projection_matrix() const
{
    return glm::perspective(glm::radians(m_Fovy), m_Aspect, +0.1f, -m_Depth);
}

glm::vec3 Viewport::get_viewport_scale(const glm::vec2& _ViewportSize) const
{
    float scaleX = 1.f / std::max<float>((float)_ViewportSize.x, 1.f);
    float scaleY = 1.f / std::max<float>((float)_ViewportSize.y, 1.f);
    return glm::vec3(scaleX, scaleY, 1.f);
}

glm::vec3 Viewport::get_axis() const
{
    return m_Axis;
}

float Viewport::get_aspect() const
{
    return m_Aspect;
}

float Viewport::get_depth() const
{
    return m_Depth;
}

float Viewport::get_fovy() const
{
    return m_Fovy;
}

void Viewport::set_axis(const glm::vec3& _Value)
{
    m_Axis = _Value;
}

void Viewport::set_aspect(const float& _Value)
{
    m_Aspect = _Value;
}

void Viewport::set_depth(const float& _Value)
{
    m_Depth = _Value;
}

void Viewport::set_fovy(const float& _Value)
{
    m_Fovy = _Value;
}