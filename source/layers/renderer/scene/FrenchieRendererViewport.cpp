#include <FrenchieRendererViewport.hpp>

using namespace Frenchie::Renderer;

Viewport::Viewport(
    const float&            _Depth,
    const float&            _Aspect,
    const float&            _Fovy,
    const glm::vec3&        _Axis,
    const glm::vec2&        _Size,
    const std::string&      _Name,
    Frenchie::Core::Object* _Parent) : 
    Frenchie::Core::Object(_Name, _Parent),
    m_Depth(_Depth), 
    m_Aspect(_Aspect), 
    m_Fovy(_Fovy), 
    m_Axis(_Axis), 
    m_Size(_Size){}

Viewport::~Viewport(){}

glm::mat4 Viewport::get_projection_matrix() const
{
    return glm::perspective(glm::radians(m_Fovy), m_Aspect, +0.1f, -m_Depth);
}

glm::vec3 Viewport::get_viewport_scale() const
{
    float scaleX = 1.f / std::max<float>((float)m_Size.x, 1.f);
    float scaleY = 1.f / std::max<float>((float)m_Size.y, 1.f);
    return glm::vec3(scaleX, scaleY, 1.f);
}

glm::vec3 Viewport::get_axis() const
{
    return m_Axis;
}

glm::vec2 Viewport::get_size() const
{
    return m_Size;
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

void Viewport::set_size(const glm::vec2& _Value)
{
    m_Size = _Value;
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