#include <FrenchieRendererScene3D.hpp>
#include <FrenchieRendererCamera.hpp>
#include <FrenchieRendererShader.hpp>
#include <FrenchieApplication.hpp>

using namespace Frenchie::Renderer;

Scene3D::Scene3D(
    const float&            _Depth,
    const float&            _Aspect,
    const float&            _Fovy,
    const glm::vec3&        _Axis,
    const glm::vec2&        _Size,
    const std::string&      _Name) : 
    Transform(_Name),
    m_Depth(_Depth), 
    m_Aspect(_Aspect), 
    m_Fovy(_Fovy), 
    m_Axis(_Axis), 
    m_Size(_Size)
    {
        create_child<Camera>(glm::vec3(+0.f, +0.f, +1.f), glm::vec3(+0.f, +1.f, +0.f), "Camera");
    }

Scene3D::~Scene3D(){}

glm::mat4 Scene3D::get_projection_matrix() const
{
    return glm::perspective(glm::radians(m_Fovy), m_Aspect, +0.1f, -m_Depth);
}

glm::vec3 Scene3D::get_viewport_scale() const
{
    float scaleX = 1.f / std::max<float>((float)m_Size.x, 1.f);
    float scaleY = 1.f / std::max<float>((float)m_Size.y, 1.f);
    return glm::vec3(scaleX, scaleY, 1.f / 1e5);
}

glm::vec3 Scene3D::get_axis() const
{
    return m_Axis;
}

glm::vec2 Scene3D::get_size() const
{
    return m_Size;
}

float Scene3D::get_aspect() const
{
    return m_Aspect;
}

float Scene3D::get_depth() const
{
    return m_Depth;
}

float Scene3D::get_fovy() const
{
    return m_Fovy;
}

void Scene3D::set_axis(const glm::vec3& _Value)
{
    m_Axis = _Value;
}

void Scene3D::set_size(const glm::vec2& _Value)
{
    m_Size = _Value;
}

void Scene3D::set_aspect(const float& _Value)
{
    m_Aspect = _Value;
}

void Scene3D::set_depth(const float& _Value)
{
    m_Depth = _Value;
}

void Scene3D::set_fovy(const float& _Value)
{
    m_Fovy = _Value;
}

void Scene3D::frame_start()
{
    Camera* camera = find_child<Camera>();

    if(camera == nullptr) // no camera --> no rendering 
        return;

    // configure all shaders that have been loaded
    auto projectionMatrix = get_projection_matrix();
    auto viewportScale    = get_viewport_scale();
    auto viewMatrix       = camera->get_view_matrix();

    Frenchie::Application::AssetManager::instance()->apply_function_to_instances<Shader>(
        [&projectionMatrix, &viewMatrix, &viewportScale](Shader* _Instance)
        {
            _Instance->begin();
            _Instance->set_uniform<glm::mat4>("u_ProjectionMatrix", projectionMatrix);
            _Instance->set_uniform<glm::mat4>("u_ViewMatrix", viewMatrix);
            _Instance->end();
        }
    );

    // setup viewport scale
    set_scale(get_viewport_scale());

    // call base implementation
    Transform::frame_start();
}