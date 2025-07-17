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
        get_object()->check_flag(Object::Flags::Focused) ||
        get_object()->check_flag(Object::Flags::Selected) ? glm::vec4(1.f, 0.0f, 0.0f, 0.1f) : glm::vec4(0.f, 1.0f, 0.0f, 0.1f));
    
    m_MeshBox->render();
    m_Shader->unuse();
}

void MeshRenderer::draw_editor(){}

std::shared_ptr<Shader> MeshRenderer::get_shader() const
{
    return m_Shader;
}

std::shared_ptr<Mesh> MeshRenderer::get_mesh() const
{
    return m_Mesh;
}


Component::TReturnType MeshRenderer::create()
{
    return std::make_unique<MeshRenderer>();
}
