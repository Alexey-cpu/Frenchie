#include <FrenchieApplicationSceneViewLayer.hpp>

#include <FrenchieRendererMeshRendererComponent.hpp>
#include <FrenchieRendererCamera.hpp>
#include <FrenchieRendererMesh.hpp>

#include <FrenchieCoreFlyweight.hpp>
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreLogger.hpp>

#include <imgui.h>
#include <imgui_internal.h>

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
    }
}

using namespace Frenchie::Application;
using namespace Frenchie::Renderer;

// SceneView
SceneView::SceneView(const std::string& _Name, std::shared_ptr<Scene3D> _Scene3D) : Layer(_Name), m_Scene(_Scene3D){}

SceneView::~SceneView(){}

bool SceneView::awake() 
{
    return m_Scene != nullptr && m_Scene->awake();
}

void SceneView::frame_start()
{
    if(m_Scene != nullptr)  
        m_Scene->frame_start();
}

void SceneView::frame_update()
{
    auto camera = 
        m_Scene != nullptr ? m_Scene->get_component<Camera>() : nullptr;

    auto size = 
        m_Scene != nullptr ? m_Scene->get_component<Size>() : nullptr;

    auto renderer = 
        m_Scene != nullptr ? m_Scene->get_component<IRenderer>() : nullptr;

    if(camera == nullptr || size == nullptr || renderer == nullptr)
        return;

    ImGui::Begin(get_name().c_str());

    // draw scene contents and update scene geometry
    {
        ImVec2 SceneWidgetPosition = ImGui::GetCursorScreenPos();
        float  SceneWidgetWidth    = ImGui::GetContentRegionAvail().x;
        float  sceneWidgetHeight   = ImGui::GetContentRegionAvail().y;
        
        ImGui::GetWindowDrawList()->AddImage(
            renderer->get_texture(), 
            ImVec2(SceneWidgetPosition.x, SceneWidgetPosition.y), 
            ImVec2(SceneWidgetPosition.x + SceneWidgetWidth, SceneWidgetPosition.y + sceneWidgetHeight), 
            ImVec2(0, 1), // in ImGUI UV coordinates are flipped
            ImVec2(1, 0)
        );

        size->set_size(glm::vec2(SceneWidgetWidth, sceneWidgetHeight));
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
            size->get_size().x, 
            size->get_size().y,
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
    auto renderer = 
        m_Scene != nullptr ? m_Scene->get_component<IRenderer>() : nullptr;

    if(renderer != nullptr) 
        renderer->render();
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