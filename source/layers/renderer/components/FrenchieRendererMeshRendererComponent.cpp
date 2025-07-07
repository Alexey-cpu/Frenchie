#include <FrenchieRendererMeshRendererComponent.hpp>

#include <FrenchieRendererScene3D.hpp>

using namespace Frenchie::Renderer;
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <imgui.h>

// MeshRenderer
MeshRenderer::MeshRenderer(
    const std::shared_ptr<Mesh>&   _Mesh, 
    const std::shared_ptr<Shader>& _Shader) :
    Component::Registry<MeshRenderer>(STRINGIFY(MeshRenderer)),
    m_Mesh(_Mesh), 
    m_Shader(_Shader),
    m_AABB(Cache<MeshBox>::request(
        "Frenchie/Mesh/MeshAABB", 
        m_Mesh->get_aabb()
        )
    )
{
}

MeshRenderer::~MeshRenderer(){}

bool MeshRenderer::awake()
{
    if(m_Mesh != nullptr && 
        m_Shader != nullptr && 
        m_Mesh->instantiate() && 
        m_Shader->instantiate())
    {

        return m_AABB->instantiate();
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

    // draw AABB
    m_Shader->use();
    m_Shader->set_uniform<glm::mat4>("u_ModelMatrix", transform->get_model_matrix());
    
    m_Shader->set_uniform<glm::vec4>(
        "u_Color",
        get_object()->check_flag(Object::Flags::Marked) ? glm::vec4(1.f, 0.0f, 0.0f, 0.3f) : glm::vec4(0.f, 1.0f, 0.0f, 0.3f));
    
    m_AABB->render();
    m_Shader->unuse();

    // update AABB geometry
}

void MeshRenderer::draw()
{
    // auto transform = 
    //     get_object() != nullptr ? 
    //         get_object()->get_component<Transform>() : 
    //             nullptr;

    // if(m_Mesh == nullptr || m_Shader == nullptr || transform == nullptr) 
    //     return;

    // //--------------------------------------------------------------------------------------------------------------
    // // local coordinates transform
    // //--------------------------------------------------------------------------------------------------------------
    // glm::vec3 scale1;
    // glm::quat rotation;
    // glm::vec3 translation;
    // glm::vec3 skew;
    // glm::vec4 perspective;
    // glm::decompose(transform->get_model_matrix(), scale1, rotation, translation, skew, perspective);

    // auto scene  = get_object()->get_parent_recursive<Scene3D>();
    // auto camera = scene->get_component<Camera>();

    // glm::vec3 cameraPos        = camera->get_position() / scene->get_viewport_scale();
    // glm::vec3 cameraDefaultPos = glm::vec3(0.f, 0.f, 1.f) / scene->get_viewport_scale();

    // auto k = std::abs(cameraDefaultPos.z / (translation.z - cameraPos.z));
    // auto scale = glm::vec3(k,k,k);

    // // get mesh axis aligned bounding box
    // auto b = m_AABB->get_aabb().transform(glm::scale(glm::mat4(1.f), scale) * transform->get_model_matrix());
    // //--------------------------------------------------------------------------------------------------------------

    // auto size = (b.Max - b.Min) * 0.5f;

    // ImGui::DragFloat3("min ", &b.Min[0], 0.5f, -10000.f, 10000.f, "%.4f");
    // ImGui::DragFloat3("max ", &b.Max[0], 0.5f, -360.f, 360.f, "%.4f");
    // ImGui::DragFloat3("size ", &size[0], 0.5f, -360.f, 360.f, "%.4f");
    // ImGui::DragFloat3("scale ", &scale[0], 0.5f, -360.f, 360.f, "%.4f");
}

std::shared_ptr<Shader> MeshRenderer::get_shader() const
{
    return m_Shader;
}

bool MeshRenderer::cast_ray(const Ray& _Ray, float _ZScale)
{
    auto transform = 
		get_object() != nullptr ? 
			get_object()->get_component<Transform>() : 
				nullptr;

    if(m_Mesh == nullptr || m_Shader == nullptr || transform == nullptr) 
        return false;

    // local coordinates transform
    return m_AABB->get_aabb().transform(
        glm::scale(glm::mat4(1.f), glm::vec3(_ZScale, _ZScale, _ZScale)) * transform->get_model_matrix()).intersects(_Ray);
}