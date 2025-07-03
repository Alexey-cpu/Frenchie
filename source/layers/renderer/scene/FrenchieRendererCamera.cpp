#include <FrenchieRendererScene3D.hpp>

#include <imgui.h>

using namespace Frenchie::Renderer;

Camera::Camera(
    glm::vec3 _CameraWorldPosition, 
    glm::vec3 _CameraWorldUpAxisDirection) :
    Component::Registry<Camera>(STRINGIFY(Camera)),
    m_CameraWorldPosition(_CameraWorldPosition), 
    m_CameraWorldUpAxisDirection(_CameraWorldUpAxisDirection){}

Camera::~Camera(){}

glm::mat4 Camera::get_view_matrix() const
{
    // camera rotation angles
    glm::mat4 rotateX  = glm::rotate(glm::mat4(1.f), glm::radians(m_Pitch), glm::vec3(1.f, 0.f, 0.f));
    glm::mat4 rotateY  = glm::rotate(glm::mat4(1.f), glm::radians(m_Yaw), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 rotateZ  = glm::rotate(glm::mat4(1.f), glm::radians(m_Roll), glm::vec3(0.f, 0.f, 1.f));

    // camera local attributes
    m_CameraLocalFrontAxisDirection = glm::vec3(0.f, 0.f, -m_Axis.z);
    m_CameraLocalFrontAxisDirection = glm::normalize(rotateY * rotateX * glm::vec4(m_CameraLocalFrontAxisDirection, 1.f));
    m_CameraLocalRightAxisDirection = glm::normalize(glm::cross(m_CameraLocalFrontAxisDirection, m_CameraWorldUpAxisDirection));
    m_CameraLocalUpAxisDirection    = glm::normalize(glm::cross(m_CameraLocalRightAxisDirection, m_CameraLocalFrontAxisDirection));

    m_CameraLocalFrontAxisDirection = glm::normalize(glm::vec3(rotateZ * glm::vec4(m_CameraLocalFrontAxisDirection, 1.f)));
    m_CameraLocalUpAxisDirection    = glm::normalize(glm::vec3(rotateZ * glm::vec4(m_CameraLocalUpAxisDirection, 1.f)));

    return glm::lookAt(m_CameraWorldPosition, m_CameraWorldPosition + m_CameraLocalFrontAxisDirection, m_CameraLocalUpAxisDirection);
}

glm::mat4 Camera::get_projection_matrix() const
{
    return glm::perspective(glm::radians(m_Fovy), m_Aspect, m_Near, m_Far);
}

glm::vec3 Camera::get_position() const
{
    Scene3D* scene3D = get_object<Scene3D>();
    auto     scale   = scene3D ? scene3D->get_viewport_scale() : glm::vec3(1.f);
    return m_CameraWorldPosition / scale;
}

glm::vec3 Camera::get_axis() const
{
    return m_Axis;
}

glm::vec3 Camera::get_front() const
{
    return m_CameraLocalFrontAxisDirection;
}

float Camera::get_aspect() const
{
    return m_Aspect;
}

float Camera::get_near() const
{
    return m_Near;
}

float Camera::get_far() const
{
    return m_Far;
}

float Camera::get_fovy() const
{
    return m_Fovy;
}

void Camera::set_axis(const glm::vec3& _Value)
{
    m_Axis = _Value;
}

float Camera::get_pitch() const
{
    return m_Pitch;
}

float Camera::get_yaw() const
{
    return m_Yaw;
}

float Camera::get_roll() const
{
    return m_Roll;
}

// settrs
void Camera::set_position(const glm::vec3& _Value)
{
    Scene3D* scene3D  = get_object<Scene3D>();
    auto     scale    = scene3D ? scene3D->get_viewport_scale() : glm::vec3(1.f);

    m_CameraWorldPosition = _Value * scale;
}

void Camera::set_pitch(const float& _Value)
{
    m_Pitch = _Value;
}

void Camera::set_yaw(const float& _Value)
{
    m_Yaw = _Value;
}

void Camera::set_roll(const float& _Value)
{
    m_Roll = _Value;
}

void Camera::set_aspect(const float& _Value)
{
    m_Aspect = _Value;
}

void Camera::set_near(const float& _Value)
{
    m_Near = _Value;
}

void Camera::set_far(const float& _Value)
{
    m_Far = _Value;
}

void Camera::set_fovy(const float& _Value)
{
    m_Fovy = _Value;
}

void Camera::draw() 
{
    auto rotation = glm::vec3(m_Pitch, m_Yaw, m_Roll);
    auto position = get_position();

    ImGui::DragFloat3("position XYZ", &position[0], 0.5f, -10000.f, 10000.f, "%.4f");
    ImGui::DragFloat3("rotation XYZ", &rotation[0], 0.5f, -360.f, 360.f, "%.4f");
    ImGui::DragFloat("Field of view", &m_Fovy, 0.1f, 0.f, 120.f, "%.4f");
    ImGui::DragFloat("Aspect", &m_Aspect, 0.1f, 0.5f, 2.f, "%.4f");
    ImGui::DragFloat("Near", &m_Near, 1.f, -10000, 10000, "%.4f");
    ImGui::DragFloat("Far", &m_Far, 1.f, -10000, 10000, "%.4f");

    set_position(position);
    set_pitch(rotation.x);
    set_yaw(rotation.y);
    set_roll(rotation.z);
}