#include <FrenchieRendererMeshRendererComponent.hpp>

using namespace Frenchie::Renderer;

#include <glm/gtx/intersect.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <imgui.h>

// MeshRenderer
MeshRenderer::MeshRenderer(
    const std::shared_ptr<Mesh>&   _Mesh, 
    const std::shared_ptr<Shader>& _Shader) :
    m_Mesh(_Mesh), 
    m_Shader(_Shader),
    m_AABB(Cache<MeshBox>::request("Frenchie/Mesh/MeshAABB", m_Mesh->get_aabb()))
{}

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

    // draw mesh AABB

    m_Shader->use();
    m_Shader->set_uniform<glm::mat4>("u_ModelMatrix", transform->get_model_matrix());
    
    m_Shader->set_uniform<glm::vec4>(
        "u_Color",
        get_object()->check_flag(Object::Flags::Marked) ? glm::vec4(1.f, 0.0f, 0.0f, 0.3f) : glm::vec4(0.f, 1.0f, 0.0f, 0.3f));
    
    m_AABB->render();
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

    // get mesh axis aligned bounding box
    auto b = m_AABB->get_aabb().transform(transform->get_model_matrix());
    ImGui::DragFloat3("min ", &b.Min[0], 0.5f, -10000.f, 10000.f, "%.4f");
    ImGui::DragFloat3("max ", &b.Max[0], 0.5f, -360.f, 360.f, "%.4f");
}

bool MeshRenderer::collide(const Ray& _Ray, glm::mat4 _View, glm::mat4 _Projection, glm::mat4 _Scale)
{
    auto transform = 
		get_object() != nullptr ? 
			get_object()->get_component<Transform>() : 
				nullptr;

    if(m_Mesh == nullptr || m_Shader == nullptr || transform == nullptr) 
        return false;

	// get mesh axis aligned bounding box
    auto b = m_AABB->get_aabb().transform(transform->get_model_matrix());

	// move ray into mesh local coordinates
	Ray  r = _Ray;

    // std::cout << " b.Min " << b.Min.x << " " << b.Min.y << "\t" << b.Min.z << "\n";
    // std::cout << " b.Max " << b.Max.x << " " << b.Max.y << "\t" << b.Max.z << "\n";


	// catch intersection
    double tmin = -INFINITY;
	double tmax = +INFINITY;

    for (int i = 0; i < 3; ++i) 
	{
        if (r.Direction[i] != 0.0) 
		{
            double t1 = (b.Min[i] - r.Origin[i]) / r.Direction[i];
            double t2 = (b.Max[i] - r.Origin[i]) / r.Direction[i];

            tmin = std::max(tmin, std::min(t1, t2));
            tmax = std::min(tmax, std::max(t1, t2));
        } 
		else if (r.Origin[i] <= b.Min[i] || r.Origin[i] >= b.Max[i]) 
		{
            std::cout << i << " r.Origin[i] " << r.Origin[i] << " b.Min " << b.Min[i] << " b.Max " << b.Max[i] << "\n";
            return false;
        }
    }

    std::cout << " tmax " << tmax << "\n";
    std::cout << " tmin " << tmin << "\n";

    return tmax > tmin && tmax > 0.0;
}