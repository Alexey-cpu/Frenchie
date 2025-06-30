#include <FrenchieRendererMeshRendererComponent.hpp>

using namespace Frenchie::Renderer;

// MeshRenderer
MeshRenderer::MeshRenderer(Mesh* _Mesh, Shader* _Shader, const std::string& _Name) : 
    Transform(_Name), 
    m_Mesh(_Mesh), m_Shader(_Shader){}

MeshRenderer::~MeshRenderer(){}

bool MeshRenderer::awake()
{
    return Transform::awake()     && 
            m_Mesh   != nullptr   && 
            m_Shader != nullptr   &&
            m_Mesh->instantiate() && 
            m_Shader->instantiate();
}

void MeshRenderer::frame_start()
{
    Transform::frame_start();
}

void MeshRenderer::frame_update()
{
    Transform::frame_update();
}

void MeshRenderer::frame_finish()
{
    Transform::frame_finish();

    if(m_Mesh == nullptr || m_Shader == nullptr) 
        return;

    m_Shader->use();
    m_Shader->set_uniform<glm::mat4>("u_ModelMatrix", m_ModelMatrix);
    m_Shader->set_uniform<glm::vec4>("u_Color", glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
    m_Mesh->render();
    m_Shader->unuse();
}