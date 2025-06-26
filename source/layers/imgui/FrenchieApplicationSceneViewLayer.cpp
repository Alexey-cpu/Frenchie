#include <FrenchieApplicationSceneViewLayer.hpp>

#include <FrenchieApplication.hpp>

#include <FrenchieRendererCamera.hpp>
#include <FrenchieRendererMesh.hpp>

#include <FrenchieCoreFlyweight.hpp>
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreLogger.hpp>

#include <imgui.h>

using namespace Frenchie::Application;
using namespace Frenchie::Renderer;

Scene::Scene(const std::string& _Name, Scene3D* _Scene3D) : Layer(_Name), m_Scene(_Scene3D)
{
    if(m_Scene == nullptr) 
        return;

    create_raw_pointer<Camera>(
        glm::vec3(+0.f, +0.f, +1.f),
        glm::vec3(+0.f, +1.f, +0.f),
        "Camera",
        m_Scene
    );
}

Scene::~Scene()
{
    glDeleteBuffers(1, &framebuffer);
    glDeleteBuffers(1, &rbo);
    glDeleteTextures(1, &textureColorbuffer);
}

bool Scene::awake() 
{
    if(m_Scene == nullptr || !m_Scene->awake()) 
        return false;

    int SCR_WIDTH = 2048;
    int SCR_HEIGHT = 1024;

    // create frame buffer
    framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    // create a color attachment texture
    textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        Logger::instance()->error("FRENCHIE::APPLICATION::IMGUI::SCENE_INSTANTIATION_FAILED");
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void Scene::frame_start()
{
    if(m_Scene != nullptr) 
        m_Scene->frame_start();
}

void Scene::frame_update()
{
    if(m_Scene == nullptr) 
        return;

    ImGui::Begin(get_name().c_str());

    m_Scene->set_size(
        glm::vec2(
            ImGui::GetContentRegionAvail().x, 
            ImGui::GetContentRegionAvail().y)
        );

    ImVec2 pos = ImGui::GetCursorScreenPos();

    ImGui::GetWindowDrawList()->AddImage(
        textureColorbuffer, 
        ImVec2(pos.x, pos.y), 
        ImVec2(pos.x + m_Scene->get_size().x, pos.y + m_Scene->get_size().y), 
        ImVec2(0, 1), 
        ImVec2(1, 0)
    );


    ImGui::End();

    m_Scene->frame_update();
}

void Scene::frame_finish()
{
    if(m_Scene == nullptr) 
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_Scene->apply_to_children_recursive(
        [](Object* _Object)
        {
            Transform* transform = 
                dynamic_cast<Transform*>(_Object);

            if(transform == nullptr || dynamic_cast<Camera*>(_Object) != nullptr) 
                return;

            transform->set_rotation(
                glm::vec3(
                    0.f,
                    0.f,
                    glm::degrees(2.f * glm::pi<float>() * 0.1f * (float)glfwGetTime())
                )
            );
        }
    );

    m_Scene->frame_finish();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
}

void Scene::finish()
{
    Layer::finish();
}

void Scene::close()
{
    Layer::close();
}

bool Scene::is_closed()
{
    return Layer::is_closed();
}