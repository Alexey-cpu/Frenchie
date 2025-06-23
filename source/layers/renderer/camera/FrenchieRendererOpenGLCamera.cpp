#include <FrenchieRendererOpenGLCamera.hpp>

using namespace Frenchie::Renderer::OpenGL;

Camera::Camera(glm::vec3 _WorldPosition, glm::vec3 _WorldUpAxisDirection) : 
    m_CameraWorldPosition(_WorldPosition), 
    m_WorldUpAxisDirection(_WorldUpAxisDirection),
    m_CameraFrontAxisDirection(glm::vec3(0.0f, 0.0f, -1.0f))
{
    updateCameraVectors();
}

Camera::~Camera(){}

glm::mat4 Camera::get_view_matrix() const
{
    return glm::lookAt(m_CameraWorldPosition, m_CameraWorldPosition + m_CameraFrontAxisDirection,  m_CameraUpAxisDirection);
}

glm::vec3 Camera::get_euler_angles() const
{
    return glm::vec3(m_Pitch, m_Yaw, m_Roll);
}

glm::vec3 Camera::get_camera_world_position() const
{
    return m_CameraWorldPosition;
}

float Camera::get_pitch() const
{
    return get_euler_angles().x;
}

float Camera::get_yaw() const
{
    return get_euler_angles().y;
}

float Camera::get_roll() const
{
    return get_euler_angles().z;
}

void Camera::set_camera_world_postion(const glm::vec3& _Value)
{
    m_CameraWorldPosition = _Value;
}

void Camera::set_yaw(const float& _Value)
{
    m_Yaw = _Value;
    updateCameraVectors();
}

void Camera::set_pitch(const float& _Value)
{
    m_Pitch = _Value;
    updateCameraVectors();
}

void Camera::set_roll(const float& _Value)
{
    m_Roll = _Value;
    updateCameraVectors();
}

void Camera::set_euler_angles(const glm::vec3& _Value)
{
    m_Pitch = _Value.x;
    m_Yaw   = _Value.y;
    m_Roll  = _Value.z;
    updateCameraVectors();
}

void Camera::move_forward(const float& _DeltaTime)
{
    m_CameraWorldPosition -= 
        m_CameraFrontAxisDirection * m_CameraMovementSpeed * _DeltaTime;
}

void Camera::move_backward(const float& _DeltaTime)
{
    m_CameraWorldPosition += 
        m_CameraFrontAxisDirection * m_CameraMovementSpeed * _DeltaTime;
}

void Camera::move_right(float _DeltaTime)
{
    m_CameraWorldPosition += 
        m_CameraRightAxisDirection * m_CameraMovementSpeed * _DeltaTime;
}

void Camera::move_left(float _DeltaTime)
{
    m_CameraWorldPosition -= 
        m_CameraRightAxisDirection * m_CameraMovementSpeed * _DeltaTime;
}

void Camera::updateCameraVectors()
{
    // rotate aroung X/Y axis
    glm::mat4 uinitMatrix = glm::mat4(1.f);
    glm::mat4 transformX  = glm::rotate(uinitMatrix, glm::radians(m_Pitch), glm::vec3(1.f, 0.f, 0.f));
    //glm::mat4 transformY  = glm::rotate(uinitMatrix, -(glm::radians(Yaw) + glm::half_pi<float>()), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 transformY  = glm::rotate(uinitMatrix, glm::radians(m_Yaw), glm::vec3(0.f, 1.f, 0.f));
    m_CameraFrontAxisDirection        = glm::normalize(transformY * transformX * glm::vec4(0.f, 0.f, -1.f, 1.f));
    m_CameraRightAxisDirection = glm::normalize(glm::cross(m_CameraFrontAxisDirection, m_WorldUpAxisDirection));
    m_CameraUpAxisDirection    = glm::normalize(glm::cross(m_CameraRightAxisDirection, m_CameraFrontAxisDirection));

    // rotate aroung Z axis
    glm::mat4 transformZ = glm::rotate(glm::mat4(1.f), glm::radians(m_Roll), glm::vec3(0.f, 0.f, 1.f));
    m_CameraFrontAxisDirection        = glm::normalize(glm::vec3(transformZ * glm::vec4(m_CameraFrontAxisDirection, 1.f)));
    m_CameraUpAxisDirection    = glm::normalize(glm::vec3(transformZ * glm::vec4(m_CameraUpAxisDirection, 1.f)));
}