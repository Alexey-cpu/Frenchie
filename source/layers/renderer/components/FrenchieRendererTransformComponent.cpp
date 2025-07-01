#include <FrenchieRendererTransformComponent.hpp>

#include <imgui.h>

#include <iostream>

using namespace Frenchie::Renderer;

Transform::Transform() : Component::Registry<Transform>(STRINGIFY(Transform)){}

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
    return Component::awake();
}

void Transform::frame_start()
{
    Component::frame_start();
}

void Transform::frame_update()
{
    auto object = get_object();

    if(object == nullptr) 
        return;

    // compute geometry
    auto parent = 
        object->get_parent() != nullptr ? 
            object->get_parent()->get_component<Transform>() : 
                nullptr;

    m_ModelMatrix = 
        parent != nullptr ? 
            parent->get_model_matrix() * compute_local_model_matrix() : 
                compute_local_model_matrix();
}

void Transform::frame_finish()
{
    Component::frame_finish();
}

void Transform::draw()
{
    ImGui::DragFloat3("position XYZ", &m_Position[0], 0.5f, -10000.f, 10000.f, "%.4f");
    ImGui::DragFloat3("rotation XYZ", &m_Rotation[0], 0.5f, -360.f, 360.f, "%.4f");
    ImGui::DragFloat3("scale XYZ", &m_Scale[0], 0.5f, 0.f, 10000, "%.4f");
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