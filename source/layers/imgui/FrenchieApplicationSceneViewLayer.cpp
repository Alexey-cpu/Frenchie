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

#include <glm/gtx/matrix_decompose.hpp>

namespace Frenchie
{
    namespace Application
    {
        class SceneViewHelpers
        {
        public:
            static glm::vec3 to_ndc(float _ScreenWidth, float _ScreenHeight, glm::vec3 _OpenGLPosition)
            {
                return glm::vec3(
                    ((float)_OpenGLPosition.x / (float)_ScreenWidth  - 0.5f) * 2.0f,
                    ((float)_OpenGLPosition.y / (float)_ScreenHeight - 0.5f) * 2.0f,
                    +1.0
                );
            }
        };

        class SceneScreenShoter : public Frenchie::Core::Component
        {
        public:
            
            SceneScreenShoter(){}
            virtual ~SceneScreenShoter()
            {
                if(m_Framebuffer != 0)
                    glDeleteBuffers(1, &m_Framebuffer);

                if(m_TextureDepthBuffer != 0)
                    glDeleteBuffers(1, &m_TextureDepthBuffer);
                
                if(m_TextureColorBuffer != 0)
                    glDeleteTextures(1, &m_TextureColorBuffer);
            }

            virtual bool awake() override
            {
            }
            
            virtual void frame_start()  override
            {
            }
            
            virtual void frame_update() override
            {
            }
            
            virtual void frame_finish() override
            {
            }
            
            virtual void draw() override
            {
            }

        protected:

            unsigned int m_Framebuffer;
            unsigned int m_TextureColorBuffer;
            unsigned int m_TextureDepthBuffer;
        };
    }
}

using namespace Frenchie::Application;
using namespace Frenchie::Renderer;

// SceneView
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
    auto camera = 
        m_Scene != nullptr ? m_Scene->get_component<Camera>() : nullptr;

    if(camera == nullptr) // no camera --> no scene --> no rendering
        return;

    ImGui::Begin(get_name().c_str());

    // draw scene contents and update scene geometry
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
        m_Scene->frame_update();
    }

    // draw scene content bounding rectangle and cast mouse cursor ray
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
        auto mousePos          = ImGui::GetMousePos();
        auto windowContentPos  = sceneViewportRect.GetTL();
        auto windowContentSize = sceneViewportRect.GetSize();
        
        auto cursorOpenGLPosition = glm::vec3(
            mousePos.x - windowContentPos.x, 
            windowContentSize.y - mousePos.y + windowContentPos.y - 1, 
            0.f
        );

        // compute cursor scene (world) position
        auto cursorNDCPosition = SceneViewHelpers::to_ndc(
            m_Scene->get_size().x, 
            m_Scene->get_size().y,
            glm::vec3(cursorOpenGLPosition.x, cursorOpenGLPosition.y, +1.f)
        );

        auto scaleMatrix             = m_Scene->get_viewport_scale_matrix();
        auto viewMatrix              = camera->get_view_matrix();
        auto projectionMatrix        = camera->get_projection_matrix();
        auto inverseConversionMatrix = glm::inverse(projectionMatrix * viewMatrix * scaleMatrix);
        auto cursorWorldPosition     = inverseConversionMatrix * glm::vec4(cursorNDCPosition, 1.f);
        auto mouseTrackerText        = fmt::format("X : {}  Y : {}", cursorWorldPosition.x, cursorWorldPosition.y);

        ImGui::GetWindowDrawList()->AddText(
            ImVec2(mousePos.x, mousePos.y) - ImGui::CalcTextSize(mouseTrackerText.c_str()), 
            IM_COL32(255, 255, 0, 255), 
            mouseTrackerText.c_str()
        );

        m_Scene->set_cursor_position(cursorWorldPosition);

        // Process mouse events ...
        if(ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)  && 
            ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            Ray ray(m_Scene->get_cursor_position(), glm::vec3(0.f, 0.f, -1.f));

            m_Scene->apply_to_children_recursive(
                [&ray, &camera](Object* _Object)
                {
                    auto meshRenderer = _Object->get_component<MeshRenderer>();
                    auto transform    = _Object->get_component<Transform>();

                    if(meshRenderer == nullptr || 
                            transform == nullptr) 
                        return;

                    _Object->set_flag(
                        Object::Flags::Marked, 
                        meshRenderer->cast_ray(
                            ray, 
                            camera->get_object_perspective_scale(transform->get_model_matrix())
                        )
                    );
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