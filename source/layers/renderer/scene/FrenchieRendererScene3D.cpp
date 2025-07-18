#include <FrenchieRendererScene3D.hpp>
#include <FrenchieRendererShader.hpp>
#include <FrenchieApplication.hpp>

// IMGUI
#include <imgui.h>


using namespace Frenchie::Renderer;

Scene3D::Scene3D(
    const glm::vec2&   _Size,
    const std::string& _Name) : 
    Core::Object(_Name),
    //m_Camera(add_component<Camera>(glm::vec3(+0.f, +0.f, +10000.f), glm::vec3(+0.f, +1.f, +0.f))),
    m_Transform(add_component<Transform>()),
    m_MousePicker(add_component<Scene3DCursor>()){}

Scene3D::~Scene3D(){}

void Scene3D::frame_start()
{
    Object::frame_start();
}

void Scene3D::frame_update()
{
    Object::frame_update();
}

void Scene3D::frame_finish()
{
    // TODO: do frustrum culling here
    Object::frame_finish();
}

void Scene3D::draw_self()
{
    // call base implementation
    Object::draw_self();

    // customize
    if(ImGui::TreeNode("Cameras"))
    {
        for(auto&& camera : m_CameraInfos)
        {
            if(ImGui::Checkbox(camera.Name.c_str(), &camera.Active))
            {
                for(auto&& notActiveCamera : m_CameraInfos)
                {
                    if(notActiveCamera.Reference != camera.Reference) 
                        notActiveCamera.Active = false;
                }
            }
        }

        ImGui::TreePop();
    }
}