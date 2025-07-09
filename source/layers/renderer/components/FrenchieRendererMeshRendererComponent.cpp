#include <FrenchieRendererMeshRendererComponent.hpp>

#include <FrenchieRendererScene3D.hpp>

using namespace Frenchie::Renderer;
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/glm.hpp>

// STL
#include <iostream>

// IMGUI
#include <imgui.h>

// MeshRenderer
MeshRenderer::MeshRenderer(
    const std::shared_ptr<Mesh>&   _Mesh, 
    const std::shared_ptr<Shader>& _Shader) :
    Component::Registry<MeshRenderer>(STRINGIFY(MeshRenderer)),
    m_Mesh(_Mesh), 
    m_Shader(_Shader),
    m_MeshBox(Cache<MeshBox>::request(
        "Frenchie/Mesh/MeshAABB", 
        m_Mesh->get_aabb()
        )
    )
{
}

MeshRenderer::~MeshRenderer(){}

bool MeshRenderer::awake()
{
    if(m_Mesh    != nullptr && 
        m_Shader != nullptr && 
        m_Mesh->instantiate() && 
        m_Shader->instantiate())
    {

        return m_MeshBox->instantiate();
    }

    return false;
}

void MeshRenderer::frame_start(){}

void MeshRenderer::frame_update(){}

void MeshRenderer::frame_finish()
{
    auto transform = 
        get_object() != nullptr ? 
            get_object()->get_component<Transform>() : 
                nullptr;

    if(m_Mesh == nullptr || m_Shader == nullptr || transform == nullptr) 
        return;

    // draw mesh
    m_Shader->use();
    m_Shader->set_uniform<glm::mat4>("u_ModelMatrix", transform->get_model_matrix());
    m_Shader->set_uniform<glm::vec4>("u_Color", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    m_Mesh->render();
    m_Shader->unuse();

    // draw MeshBox
    m_Shader->use();
    m_Shader->set_uniform<glm::mat4>("u_ModelMatrix", transform->get_model_matrix());
    
    m_Shader->set_uniform<glm::vec4>(
        "u_Color",
        get_object()->check_flag(Object::Flags::Marked) || 
        get_object()->check_flag(Object::Flags::Focused) ||
        get_object()->check_flag(Object::Flags::Selected) ? glm::vec4(1.f, 0.0f, 0.0f, 0.1f) : glm::vec4(0.f, 1.0f, 0.0f, 0.1f));
    
    m_MeshBox->render();
    m_Shader->unuse();
}

void MeshRenderer::draw()
{
    auto transform = 
        get_object() != nullptr ? 
            get_object()->get_component<Transform>() : 
                nullptr;

    if(m_Mesh == nullptr || m_Shader == nullptr || transform == nullptr) 
        return;

    //--------------------------------------------------------------------------------------------------------------
    // local coordinates transform
    //--------------------------------------------------------------------------------------------------------------
    auto scale = get_object()->get_parent_recursive<Scene3D>()->get_component<Camera>()->get_object_perspective_scale(transform->get_model_matrix());

    // get mesh axis aligned bounding box
    auto b = m_Mesh->get_aabb().transform(glm::scale(glm::mat4(1.f), scale) * transform->get_model_matrix());
    //--------------------------------------------------------------------------------------------------------------

    auto size = (b.Max - b.Min) * 0.5f;

    ImGui::DragFloat3("min ", &b.Min[0], 0.5f, -10000.f, 10000.f, "%.4f");
    ImGui::DragFloat3("max ", &b.Max[0], 0.5f, -360.f, 360.f, "%.4f");
    ImGui::DragFloat3("size ", &size[0], 0.5f, -360.f, 360.f, "%.4f");
    ImGui::DragFloat3("scale ", &scale[0], 0.5f, -360.f, 360.f, "%.4f");
}

std::shared_ptr<Shader> MeshRenderer::get_shader() const
{
    return m_Shader;
}

bool MeshRenderer::cast_ray(const Ray& _Ray, glm::vec3 _ZScale)
{
    auto transform = 
		get_object() != nullptr ? 
			get_object()->get_component<Transform>() : 
				nullptr;

    if(m_Mesh == nullptr || m_Shader == nullptr || transform == nullptr) 
        return false;

    // local coordinates transform
    return m_MeshBox->get_aabb().transform(
        glm::scale(glm::mat4(1.f), _ZScale) * transform->get_model_matrix()).intersects(_Ray);
}