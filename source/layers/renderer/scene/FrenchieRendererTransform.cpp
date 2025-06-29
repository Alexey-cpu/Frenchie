#include <FrenchieRendererTransform.hpp>

using namespace Frenchie::Renderer;

Transform::Transform(const std::string& _Name, Hierarchy* _Parent) : Hierarchy(_Name, _Parent){}

Transform::~Transform(){}

glm::vec3 Transform::get_position() const
{
    return m_Position;
}

glm::vec3 Transform::get_rotation() const
{
    return m_Rotation;
}

glm::vec3 Transform::get_scale() const
{
    return m_Scale;
}

glm::mat4 Transform::get_model_matrix() const
{
    return m_ModelMatrix;
}

void Transform::set_position(const glm::vec3& _Value)
{
    m_Position = _Value;
}

void Transform::set_rotation(const glm::vec3& _Value)
{
    m_Rotation = _Value;
}

void Transform::set_scale(const glm::vec3& _Value)
{
    m_Scale = _Value;
}

bool Transform::awake()
{
    for(auto&& child : m_Children)
    {
        Transform* childTransform = dynamic_cast<Transform*>(child);

        if(childTransform != nullptr)
            childTransform->awake();
    }

    return true;
}

void Transform::frame_start()
{
    for(auto&& child : m_Children)
    {
        Transform* childTransform = dynamic_cast<Transform*>(child);

        if(childTransform != nullptr)
            childTransform->frame_start();
    }
}

void Transform::frame_update()
{
    // compute geometry
    auto parent = get_parent<Transform>();

    m_ModelMatrix = 
        parent != nullptr ? 
            parent->m_ModelMatrix * compute_local_model_matrix() : 
                compute_local_model_matrix();

    for(auto&& child : m_Children)
    {
        Transform* childTransform = dynamic_cast<Transform*>(child);

        if(childTransform != nullptr)
            childTransform->frame_update();
    }
}

void Transform::frame_finish()
{
    for(auto&& child : m_Children)
    {
        Transform* childTransform = dynamic_cast<Transform*>(child);

        if(childTransform != nullptr)
            childTransform->frame_finish();
    }
}

glm::mat4 Transform::compute_local_model_matrix() const
{
    glm::mat4 matrix(1.f);

    return glm::translate(matrix, m_Position) * 
            glm::rotate(matrix, glm::radians(m_Rotation.x), glm::vec3(1.f, 0.f, 0.f)) * 
            glm::rotate(matrix, glm::radians(m_Rotation.y), glm::vec3(0.f, 1.f, 0.f)) * 
            glm::rotate(matrix, glm::radians(m_Rotation.z), glm::vec3(0.f, 0.f, 1.f)) * 
            glm::scale(matrix, m_Scale);
}