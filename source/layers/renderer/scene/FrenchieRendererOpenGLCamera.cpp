#include <FrenchieRendererOpenGLCamera.hpp>
#include <FrenchieRendererOpenGLViewport.hpp>

using namespace Frenchie::Renderer::OpenGL;

Camera::Camera(
    glm::vec3 _CameraWorldPosition, 
    glm::vec3 _CameraWorldUpAxisDirection, 
    const std::string& _Name, 
    Frenchie::Core::Object* _Parent) : 
    Frenchie::Core::Object(_Name, _Parent),
    m_CameraWorldPosition(_CameraWorldPosition), 
    m_CameraWorldUpAxisDirection(_CameraWorldUpAxisDirection){}

Camera::~Camera(){}

glm::vec3 Camera::get_position() const
{
    return m_CameraWorldPosition;
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

glm::mat4 Camera::get_view_matrix() const
{
    Viewport* viewport = 
        get_parent_recursive<Viewport>();
    
    if(viewport == nullptr) 
        return glm::mat4(1.f);

    // camera rotation angles
    glm::mat4 rotateX  = glm::rotate(glm::mat4(1.f), glm::radians(m_Pitch), glm::vec3(1.f, 0.f, 0.f));
    glm::mat4 rotateY  = glm::rotate(glm::mat4(1.f), glm::radians(m_Yaw), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 rotateZ  = glm::rotate(glm::mat4(1.f), glm::radians(m_Roll), glm::vec3(0.f, 0.f, 1.f));

    // camera local attributes
    m_CameraLocalFrontAxisDirection = glm::vec3(0.f, 0.f, -viewport->get_axis().z);
    m_CameraLocalFrontAxisDirection = glm::normalize(rotateY * rotateX * glm::vec4(m_CameraLocalFrontAxisDirection, 1.f));
    m_CameraLocalRightAxisDirection = glm::normalize(glm::cross(m_CameraLocalFrontAxisDirection, m_CameraWorldUpAxisDirection));
    m_CameraLocalUpAxisDirection    = glm::normalize(glm::cross(m_CameraLocalRightAxisDirection, m_CameraLocalFrontAxisDirection));

    m_CameraLocalFrontAxisDirection = glm::normalize(glm::vec3(rotateZ * glm::vec4(m_CameraLocalFrontAxisDirection, 1.f)));
    m_CameraLocalUpAxisDirection    = glm::normalize(glm::vec3(rotateZ * glm::vec4(m_CameraLocalUpAxisDirection, 1.f)));

    return glm::lookAt(m_CameraWorldPosition, m_CameraWorldPosition + m_CameraLocalFrontAxisDirection, m_CameraLocalUpAxisDirection);
}