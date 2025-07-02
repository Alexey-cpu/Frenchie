#include <FrenchieApplicationSceneViewLayer.hpp>

#include <FrenchieApplication.hpp>

#include <FrenchieRendererCamera.hpp>
#include <FrenchieRendererMesh.hpp>

#include <FrenchieCoreFlyweight.hpp>
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreLogger.hpp>

#include <imgui.h>
#include <imgui_internal.h>

using namespace Frenchie::Application;
using namespace Frenchie::Renderer;

SceneView::SceneView(const std::string& _Name, std::shared_ptr<Scene3D> _Scene3D) : Layer(_Name), m_Scene(_Scene3D){}

SceneView::~SceneView()
{
    if(m_Framebuffer != 0)
        glDeleteBuffers(1, &m_Framebuffer);

    if(m_TextureDepthBuffer != 0)
        glDeleteBuffers(1, &m_TextureDepthBuffer);
    
    if(m_TextureColorBuffer != 0)
        glDeleteTextures(1, &m_TextureColorBuffer);
}

bool SceneView::awake() 
{
    if(m_Scene == nullptr || !m_Scene->awake()) 
        return false;

    int SCR_WIDTH = 2048;
    int SCR_HEIGHT = 1024;

    // create frame buffer
    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    
    // create a color attachment texture
    glGenTextures(1, &m_TextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorBuffer, 0);
    
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &m_TextureDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_TextureDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_TextureDepthBuffer); // now actually attach it
    
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

    // draw scene
    {
        ImVec2 windowScreenPosition = ImGui::GetCursorScreenPos();
        float  width                = ImGui::GetContentRegionAvail().x;
        float  height               = ImGui::GetContentRegionAvail().y;
        
        ImGui::GetWindowDrawList()->AddImage(
            m_TextureColorBuffer, 
            ImVec2(windowScreenPosition.x, windowScreenPosition.y), 
            ImVec2(windowScreenPosition.x + width, windowScreenPosition.y + height), 
            ImVec2(0, 1), // in ImGUI UV coordinates are flipped
            ImVec2(1, 0)
        );

        m_Scene->set_size(glm::vec2(width, height));
    }

    // draw scene content bounding rectangle
    {
        // draw scene bounding rectangle
        ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        ImVec2 vMax = ImGui::GetWindowContentRegionMax();

        vMin.x += ImGui::GetWindowPos().x;
        vMin.y += ImGui::GetWindowPos().y;
        vMax.x += ImGui::GetWindowPos().x;
        vMax.y += ImGui::GetWindowPos().y;
        ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 255, 0, 255));

        // compute scene cursor position
        ImRect content           = ImRect(vMin, vMax);
        ImVec2 mousePos          = ImGui::GetMousePos();
        ImVec2 windowContentPos  = content.GetTL();
        ImVec2 windowContentSize = content.GetSize();

        // Adjust mouse coordinates relative to the scene window
        float adjustedX = mousePos.x - windowContentPos.x;
        float adjustedY = mousePos.y - windowContentPos.y;

        // Flip Y-coordinate for OpenGL (assuming 0,0 is bottom-left in OpenGL)
        m_Scene->set_cursor_postion(
            glm::vec3(
                adjustedX, 
                windowContentSize.y - adjustedY - 1, 
                0.f // TODO: identify Z-component somehow
            )
        );

        //
        float width  = m_Scene->get_size().x;
        float height = m_Scene->get_size().y;
        auto cursor     = m_Scene->get_cursor_position();
        auto x_norm     = (2.f * cursor.x / width - 1.f);
        auto y_norm     = (2.f * cursor.y / height - 1.f);
        auto projection = m_Scene->get_component<Camera>()->get_projection_matrix();
        auto view       = m_Scene->get_component<Camera>()->get_view_matrix();
        auto scale      = m_Scene->get_viewport_scale_matrix();
        auto axis       = m_Scene->get_component<Camera>()->get_axis();
        auto ray        = glm::inverse(view) * glm::inverse(projection) * glm::inverse(scale) * glm::vec4(x_norm, y_norm, -axis.z, 1.f);

        auto text = fmt::format("{} {}", ray.x, ray.y);

        ImGui::GetWindowDrawList()->AddText(
            ImVec2(mousePos.x, mousePos.y) - ImGui::CalcTextSize(text.c_str()), 
            IM_COL32(255, 255, 0, 255), 
            fmt::format("{} {}", ray.x, ray.y).c_str()
        );
    }

    // Handle scene mouse events
    if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
    {
    }

    ImGui::End();
    
    m_Scene->frame_update();
}

void SceneView::frame_finish()
{
    if(m_Scene == nullptr) 
        return;

    float width  = m_Scene->get_size().x;
    float height = m_Scene->get_size().y;

    // resize frame buffer
    glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)width, (int)height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorBuffer, 0);
	
    glBindRenderbuffer(GL_RENDERBUFFER, m_TextureDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)width, (int)height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_TextureDepthBuffer);

    // bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glViewport(0, 0, (int)width, (int)height);

    m_Scene->frame_finish();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_FRAMEBUFFER, 0);

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