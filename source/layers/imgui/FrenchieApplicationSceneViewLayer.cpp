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

SceneView::SceneView(const std::string& _Name, Scene3D* _Scene3D) : Layer(_Name), m_Scene(_Scene3D){}

SceneView::~SceneView()
{
    glDeleteBuffers(1, &m_Framebuffer);
    glDeleteBuffers(1, &m_RBO);
    glDeleteTextures(1, &m_TextureColorBuffer);
}

bool SceneView::awake() 
{
    if(m_Scene == nullptr || !m_Scene->awake()) 
        return false;

    int SCR_WIDTH = 2048;
    int SCR_HEIGHT = 1024;

    // create frame buffer
    m_Framebuffer;
    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    
    // create a color attachment texture
    m_TextureColorBuffer;
    glGenTextures(1, &m_TextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorBuffer, 0);
    
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    m_RBO;
    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO); // now actually attach it
    
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        Logger::instance()->error("FRENCHIE::APPLICATION::IMGUI::SCENE_INSTANTIATION_FAILED");
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void SceneView::frame_start()
{
    if(m_Scene == nullptr) 
        return;
    
    m_Scene->frame_start();
}

void SceneView::frame_update()
{
    if(m_Scene == nullptr) 
        return;

    ImGui::Begin(get_name().c_str());

    ImVec2 current_size = ImGui::GetWindowSize();

    int width  = (int)ImGui::GetContentRegionAvail().x;
    int height = (int)ImGui::GetContentRegionAvail().y;

    // resize scene
    m_Scene->set_size(glm::vec2(width, height));

    ImVec2 pos = ImGui::GetCursorScreenPos();

    ImGui::GetWindowDrawList()->AddImage(
        m_TextureColorBuffer, 
        ImVec2(pos.x, pos.y), 
        ImVec2(pos.x + width, pos.y + height), 
        ImVec2(0, 1), // in ImGUI UV coordinates are flipped
        ImVec2(1, 0)
    );


    ImGui::End();

    m_Scene->frame_update();
}

void SceneView::frame_finish()
{
    if(m_Scene == nullptr) 
        return;

    int width  = (int)m_Scene->get_size().x;
    int height = (int)m_Scene->get_size().y;

    // resize frame buffer
    glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorBuffer, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

    // bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glViewport(0, 0, width, height);

    // This is the test code !!!

    // m_Scene->apply_to_children_recursive(
    // [](Object* _Object)
    // {
    //     Transform* transform = 
    //         dynamic_cast<Transform*>(_Object);

    //     if(transform == nullptr || dynamic_cast<Camera*>(_Object) != nullptr) 
    //         return;

    //     transform->set_rotation(
    //         glm::vec3(
    //             0.f,
    //             glm::degrees(2.f * glm::pi<float>() * 0.1f * (float)glfwGetTime()),
    //             0.f
    //         )
    //     );
    // }
    // );

    m_Scene->frame_finish();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
}

void SceneView::finish()
{
    Layer::finish();
}

void SceneView::close()
{
    Layer::close();
}

bool SceneView::is_closed()
{
    return Layer::is_closed();
}