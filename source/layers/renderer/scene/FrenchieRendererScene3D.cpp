#include <FrenchieRendererScene3D.hpp>
#include <FrenchieRendererCamera.hpp>
#include <FrenchieRendererShader.hpp>
#include <FrenchieApplication.hpp>

using namespace Frenchie::Renderer;

Scene3D::Scene3D(
    const glm::vec2&   _Size,
    const std::string& _Name) : 
    Core::Object(_Name),
    m_Size(_Size), 
    m_Camera(add_component<Camera>(glm::vec3(+0.f, +0.f, +1.f), glm::vec3(+0.f, +1.f, +0.f))),
    m_Transform(add_component<Transform>()){}

Scene3D::~Scene3D(){}

glm::vec3 Scene3D::get_viewport_scale() const
{
    float scaleX = 1.f / std::max<float>((float)m_Size.x, 1.f);
    float scaleY = 1.f / std::max<float>((float)m_Size.y, 1.f);
    return glm::vec3(scaleX, scaleY, std::max<float>(scaleX, scaleY));
}

glm::vec3 Scene3D::get_cursor_position() const
{
    return m_CursorPosition;
}

glm::vec2 Scene3D::get_size() const
{
    return m_Size;
}

void Scene3D::set_size(const glm::vec2& _Value)
{
    m_Size = _Value;
}

void Scene3D::set_cursor_postion(const glm::vec3& _Value)
{
    m_CursorPosition = _Value;
}

void Scene3D::frame_start()
{
    if(m_Camera == nullptr || m_Transform == nullptr) // no camera or no transform --> no rendering 
        return;

    apply_to_children_recursive([this](Object* _Objcet)
    {
        auto component = _Objcet->get_component<IShader>();
        auto shader    = component != nullptr ? component->get_shader() : nullptr;

        if(shader == nullptr) 
            return;

        shader->use();
        shader->set_uniform<glm::mat4>("u_ProjectionMatrix", m_Camera->get_projection_matrix());
        shader->set_uniform<glm::mat4>("u_ViewMatrix", m_Camera->get_view_matrix());
        shader->unuse();
    }
    );

    // setup viewport scale
    m_Transform->set_scale(get_viewport_scale());

    // call base implementation
    Object::frame_start();
}