#include <FrenchieRendererMeshRendererComponent.hpp>

using namespace Frenchie::Renderer;

// MeshRenderer
MeshRenderer::MeshRenderer(Mesh* _Mesh, Shader* _Shader) : 
    m_Mesh(_Mesh), m_Shader(_Shader){}

MeshRenderer::~MeshRenderer(){}

bool MeshRenderer::awake()
{
    return  m_Mesh   != nullptr   && 
            m_Shader != nullptr   &&
            m_Mesh->instantiate() && 
            m_Shader->instantiate();
}

void MeshRenderer::frame_start(){}

void MeshRenderer::frame_update(){}

void MeshRenderer::frame_finish()
{
    auto object = get_object();
    auto transform = object != nullptr ? object->get_component<Transform>() : nullptr;

    if(m_Mesh == nullptr || m_Shader == nullptr || transform == nullptr) 
        return;

    m_Shader->use();
    m_Shader->set_uniform<glm::mat4>("u_ModelMatrix", transform->get_model_matrix());
    m_Shader->set_uniform<glm::vec4>("u_Color", glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
    m_Mesh->render();
    m_Shader->unuse();
}