#include <FrenchieApplicationSceneViewLayer.hpp>

#include <FrenchieApplication.hpp>

#include <FrenchieRendererCamera.hpp>
#include <FrenchieRendererMesh.hpp>

#include <FrenchieCoreFlyweight.hpp>
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreLogger.hpp>

#include <imgui.h>
#include <imgui_internal.h>

#include <FrenchieRendererMeshRendererComponent.hpp>

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
        ImVec2 SceneWidgetPosition = ImGui::GetCursorScreenPos();
        float  SceneWidgetWidth    = ImGui::GetContentRegionAvail().x;
        float  sceneWidgetHeight   = ImGui::GetContentRegionAvail().y;
        
        ImGui::GetWindowDrawList()->AddImage(
            m_TextureColorBuffer, 
            ImVec2(SceneWidgetPosition.x, SceneWidgetPosition.y), 
            ImVec2(SceneWidgetPosition.x + SceneWidgetWidth, SceneWidgetPosition.y + sceneWidgetHeight), 
            ImVec2(0, 1), // in ImGUI UV coordinates are flipped
            ImVec2(1, 0)
        );

        m_Scene->set_size(glm::vec2(SceneWidgetWidth, sceneWidgetHeight));
    }

    // draw scene content bounding rectangle
    {
        // draw scene scene viewport rect rectangle
        ImRect sceneViewportRect = 
            ImRect(
                ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos(), 
                ImGui::GetWindowContentRegionMax() + ImGui::GetWindowPos());

        ImGui::GetForegroundDrawList()->AddRect(
            sceneViewportRect.Min, 
            sceneViewportRect.Max, 
            IM_COL32(255, 255, 0, 255)
        );

        // compute cursor OpenGL position
        ImVec2 mousePos          = ImGui::GetMousePos();
        ImVec2 windowContentPos  = sceneViewportRect.GetTL();
        ImVec2 windowContentSize = sceneViewportRect.GetSize();
        
        glm::vec3 cursorOpenGLPosition = glm::vec3(
            mousePos.x - windowContentPos.x, 
            windowContentSize.y - mousePos.y + windowContentPos.y - 1, 
            0.f
        );

        // compute cursor scene (world position)
        auto cursorNDCPosition = glm::vec3(
            (2.f * cursorOpenGLPosition.x / m_Scene->get_size().x - 1.f),
            (2.f * cursorOpenGLPosition.y / m_Scene->get_size().y - 1.f),
            -1.f
        );

        auto scaleMatrix         = m_Scene->get_viewport_scale_matrix();
        auto viewMatrix          = m_Scene->get_component<Camera>()->get_view_matrix();
        auto projectionMatrix    = m_Scene->get_component<Camera>()->get_projection_matrix();
        auto cursorWorldPosition = glm::inverse(scaleMatrix) * glm::inverse(viewMatrix) * glm::inverse(projectionMatrix) * glm::vec4(cursorNDCPosition, 1.f);
        
        auto mouseTrackerText    = fmt::format("{} {} {}", cursorWorldPosition.x, cursorWorldPosition.y, cursorWorldPosition.z);

        ImGui::GetWindowDrawList()->AddText(
            ImVec2(mousePos.x, mousePos.y) - ImGui::CalcTextSize(mouseTrackerText.c_str()), 
            IM_COL32(255, 255, 0, 255), 
            mouseTrackerText.c_str()
        );

        m_Scene->set_cursor_postion(cursorWorldPosition);

        m_Scene->frame_update();

        //Handle scene mouse events
        if(ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)  && 
            ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            float screenWidth   = m_Scene->get_size().x;
            float screenHeight  = m_Scene->get_size().y;

            // The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
            glm::vec4 lRayStart_NDC(
                ((float)cursorOpenGLPosition.x / (float)screenWidth  - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
                ((float)cursorOpenGLPosition.y / (float)screenHeight - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
                -1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
                1.0f
            );
            glm::vec4 lRayEnd_NDC(
                ((float)cursorOpenGLPosition.x / (float)screenWidth  - 0.5f) * 2.0f,
                ((float)cursorOpenGLPosition.y / (float)screenHeight - 0.5f) * 2.0f,
                0.0,
                1.0f
            );

            glm::vec3 lRayDir_world   = glm::inverse(scaleMatrix) * glm::inverse(viewMatrix) * glm::inverse(projectionMatrix) * (lRayEnd_NDC - lRayStart_NDC);
            glm::vec3 lRayStart_world = glm::inverse(scaleMatrix) * glm::inverse(viewMatrix) * glm::inverse(projectionMatrix) * lRayStart_NDC;

            // std::cout << "lRayStart_world " << lRayDir_world.x << "\t" << lRayDir_world.y << "\t" << lRayDir_world.z << "\n";

            Ray rayObj(lRayStart_world, lRayDir_world);

            m_Scene->apply_to_children_recursive(
                [&rayObj, &scaleMatrix, &viewMatrix, &projectionMatrix](Object* _Object)
                {
                    auto meshRenderer = _Object->get_component<MeshRenderer>();

                    if(meshRenderer == nullptr) 
                        return;

                    std::cout << "checking object " << _Object->get_name() << "\n";

                    _Object->set_flag(
                        Object::Flags::Marked, 
                        meshRenderer->collide(rayObj, viewMatrix, projectionMatrix, scaleMatrix));
                }
            );
        }
    }

    ImGui::End();
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

    // blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glViewport(0, 0, (int)width, (int)height);

    m_Scene->frame_finish();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_FRAMEBUFFER, 0);

    glDisable(GL_BLEND);
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