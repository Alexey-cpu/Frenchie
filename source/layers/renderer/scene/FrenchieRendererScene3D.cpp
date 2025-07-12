#include <FrenchieRendererScene3D.hpp>
#include <FrenchieRendererCamera.hpp>
#include <FrenchieRendererShader.hpp>
#include <FrenchieApplication.hpp>

using namespace Frenchie::Renderer;

Scene3D::Scene3D(
    const glm::vec2&   _Size,
    const std::string& _Name) : 
    Core::Object(_Name),
    m_Size(add_component<Size>(_Size)), 
    m_Camera(add_component<Camera>(glm::vec3(+0.f, +0.f, +10000.f), glm::vec3(+0.f, +1.f, +0.f))),
    m_Transform(add_component<Transform>()),
    m_MousePicker(add_component<Scene3DMousePicker>())
    {
        add_component<ScreenShoter>();
    }

Scene3D::~Scene3D(){}

void Scene3D::frame_start()
{
    if(m_Camera == nullptr || m_Transform == nullptr) // no camera or no transform --> no rendering 
        return;

    auto projectionMatrix = m_Camera->get_projection_matrix() * m_Camera->get_view_matrix();

    apply_to_children_recursive(
        [this, &projectionMatrix](Object* _Objcet)
        {
            auto component = _Objcet->get_component<IShader>();
            auto shader    = component != nullptr ? component->get_shader() : nullptr;

            if(shader == nullptr) 
                return;

            shader->use();
            shader->set_uniform<glm::mat4>("u_ProjectionMatrix", projectionMatrix);
            shader->unuse();
        }
    );

    // call base implementation
    Object::frame_start();
}

void Scene3D::frame_update()
{
    // setup scale
    auto size = m_Size->get_size();
    
    get_component<Transform>()->set_scale(
        glm::vec3(
            1.f / std::max<float>((float)size.x, 1.f), 
            1.f / std::max<float>((float)size.y, 1.f), 
            1.f / 10000.f
        )
    );

    Object::frame_update();
}

void Scene3D::frame_finish()
{
    // TODO: do frustrum culling here
    Object::frame_finish();
}