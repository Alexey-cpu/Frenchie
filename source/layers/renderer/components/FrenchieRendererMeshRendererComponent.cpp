#include <FrenchieRendererMeshRendererComponent.hpp>

#include <FrenchieRendererScene3D.hpp>

#include <FrenchieCoreCache.hpp>

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
        //get_object()->check_flag(Object::Flags::Marked) || 
        get_object()->check_flag(Object::Flags::Focused) ||
        get_object()->check_flag(Object::Flags::Selected) ? glm::vec4(1.f, 0.0f, 0.0f, 0.1f) : glm::vec4(0.f, 1.0f, 0.0f, 0.1f));
    
    m_MeshBox->render();
    m_Shader->unuse();
}

void MeshRenderer::draw_editor()
{
    auto transform = 
        get_object() != nullptr ? 
            get_object()->get_component<Transform>() : 
                nullptr;

    if(m_Mesh == nullptr || m_Shader == nullptr || transform == nullptr) 
        return;

    auto scene  = get_object()->get_parent_recursive<Scene3D>();
    auto camera = scene->get_component<Camera>();

    // retrieve viewport and camera transform matrixes
    auto viewportScaleMatrix    = scene->get_viewport_scale_matrix();
    auto cameraViewMatrix       = camera->get_view_matrix();
    auto cameraProjectionMatrix = camera->get_projection_matrix();
    auto screenTransformMatrix  = cameraProjectionMatrix * cameraViewMatrix * viewportScaleMatrix;

    auto modelMatrix         = transform->get_model_matrix();
    auto aabbTransformMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.f / (screenTransformMatrix * modelMatrix)[3][3]));

    auto b = get_mesh()->get_aabb().transform(aabbTransformMatrix * modelMatrix);

    auto size = (b.Max - b.Min) * 0.5f;

    ImGui::DragFloat3("min ", &b.Min[0], 0.5f, -10000.f, 10000.f, "%.4f");
    ImGui::DragFloat3("max ", &b.Max[0], 0.5f, -360.f, 360.f, "%.4f");
    ImGui::DragFloat3("size ", &size[0], 0.5f, -360.f, 360.f, "%.4f");

    // ImGui::DragFloat3("scale ", &scale[0], 0.5f, -360.f, 360.f, "%.4f");
    // ImGui::DragFloat3("translation ", &translation[0], 0.5f, -360.f, 360.f, "%.4f");
}

std::shared_ptr<Shader> MeshRenderer::get_shader() const
{
    return m_Shader;
}

// bool MeshRenderer::cast_ray(const Ray& _Ray, glm::mat4 _AAABTransform)
// {
//     auto transform = 
// 		get_object() != nullptr ? 
// 			get_object()->get_component<Transform>() : 
// 				nullptr;

//     if(m_Mesh == nullptr || m_Shader == nullptr || transform == nullptr) 
//         return false;

//     // local coordinates transform
//     return m_MeshBox->get_aabb().transform(
//         _AAABTransform * transform->get_model_matrix()).intersects(_Ray);
//}