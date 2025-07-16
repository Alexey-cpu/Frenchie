#include <FrenchieRendererCamera.hpp>
#include <FrenchieRendererScene3D.hpp>
#include <imgui.h>

using namespace Frenchie::Renderer;

Camera::Camera(
    glm::vec3 _CameraWorldPosition, 
    glm::vec3 _CameraWorldUpAxisDirection) :
    Component::Registry<Camera>(STRINGIFY(Camera)),
    m_CameraWorldPosition(_CameraWorldPosition), 
    m_CameraWorldUpAxisDirection(_CameraWorldUpAxisDirection)
{

}

Camera::~Camera(){}

glm::mat4 Camera::get_view_matrix() const
{
    // camera rotation angles
    glm::mat4 rotateX  = glm::rotate(glm::mat4(1.f), glm::radians(m_Pitch), glm::vec3(1.f, 0.f, 0.f));
    glm::mat4 rotateY  = glm::rotate(glm::mat4(1.f), glm::radians(m_Yaw), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 rotateZ  = glm::rotate(glm::mat4(1.f), glm::radians(m_Roll), glm::vec3(0.f, 0.f, 1.f));

    // camera local attributes
    m_CameraLocalFrontAxisDirection = glm::normalize(rotateY * rotateX * glm::vec4(glm::vec3(0.f, 0.f, -m_Axis.z), 1.f));
    m_CameraLocalRightAxisDirection = glm::normalize(glm::cross(m_CameraLocalFrontAxisDirection, m_CameraWorldUpAxisDirection));
    m_CameraLocalUpAxisDirection    = glm::normalize(glm::cross(m_CameraLocalRightAxisDirection, m_CameraLocalFrontAxisDirection));
    
    // rotate around Z axis
    m_CameraLocalFrontAxisDirection = glm::normalize(glm::vec3(rotateZ * glm::vec4(m_CameraLocalFrontAxisDirection, 1.f)));
    m_CameraLocalUpAxisDirection    = glm::normalize(glm::vec3(rotateZ * glm::vec4(m_CameraLocalUpAxisDirection, 1.f)));

    Object*   object          = get_object<Object>();
    glm::mat4 transformMatrix = object != nullptr ? object->get_component<Transform>()->get_model_matrix() : glm::mat4(1.f);
    glm::vec3 cameraPosition  = transformMatrix * glm::vec4(m_CameraWorldPosition, 1.f);

    return glm::lookAt(
        cameraPosition, 
        cameraPosition + m_CameraLocalFrontAxisDirection, 
        m_CameraLocalUpAxisDirection);
}

glm::mat4 Camera::get_projection_matrix() const
{
    return glm::perspective(
        glm::radians(m_Fovy), 
        m_Aspect, 
        m_Near, 
        m_Far
    );
}

glm::vec3 Camera::get_position() const
{
    return m_CameraWorldPosition;
}

glm::vec3 Camera::get_axis() const
{
    return m_Axis;
}

glm::vec3 Camera::get_front() const
{
    return m_CameraLocalFrontAxisDirection;
}

glm::vec3 Camera::get_up() const
{
    return m_CameraLocalUpAxisDirection;
}

glm::vec3 Camera::get_right() const
{
    return m_CameraLocalRightAxisDirection;
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

float Camera::get_movement_speed() const
{
    return m_MovementSpeed;
}

float Camera::get_sensitivity() const
{
    return m_Sensitivity;
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

void Camera::set_position(const glm::vec3& _Value)
{
    m_CameraWorldPosition = _Value;
}

void Camera::set_movement_speed(const float& _Value)
{
    m_MovementSpeed = _Value;
}

void Camera::set_sensitivity(const float& _Value)
{
    m_Sensitivity = _Value;
}

void Camera::set_pitch(const float& _Value)
{
    if(_Value > 89.0f) 
        m_Pitch = 89.0f;
    else if(_Value < -89.0f) 
        m_Pitch = -89.0f;
    else 
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

void Camera::draw_editor() 
{
    auto rotation = glm::vec3(m_Pitch, m_Yaw, m_Roll);

    ImGui::DragFloat3("position XYZ", &m_CameraWorldPosition[0], 0.5f, -10000.f, 10000.f, "%.4f");
    ImGui::DragFloat3("rotation XYZ", &rotation[0], 0.5f, -360.f, 360.f, "%.4f");
    ImGui::DragFloat("Field of view", &m_Fovy, 0.1f, 0.f, 120.f, "%.4f");
    ImGui::DragFloat("Movement speed", &m_MovementSpeed, 0.5f, 1.0f, 10.f, "%.4f");
    ImGui::DragFloat("Sensitivity", &m_Sensitivity, 0.01f, 0.001f, 1.f, "%.4f");
    ImGui::DragFloat("Aspect", &m_Aspect, 0.1f, 0.5f, 2.f, "%.4f");
    ImGui::DragFloat("Near", &m_Near, 1.f, -10000, 10000, "%.4f");
    ImGui::DragFloat("Far", &m_Far, 1.f, -10000, 10000, "%.4f");

    set_pitch(rotation.x);
    set_yaw(rotation.y);
    set_roll(rotation.z);
}

Component::TReturnType Camera::create()
{
    return std::make_unique<Camera>();
}