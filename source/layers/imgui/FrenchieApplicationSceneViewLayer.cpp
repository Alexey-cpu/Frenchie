#include <FrenchieApplicationSceneViewLayer.hpp>

#include <FrenchieApplication.hpp>

#include <FrenchieRendererMeshRendererComponent.hpp>
#include <FrenchieRendererCamera.hpp>
#include <FrenchieRendererMesh.hpp>

#include <FrenchieRendererIRenderer.hpp>

#include <FrenchieCoreLogger.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtx/matrix_decompose.hpp>

using namespace Frenchie::Application;
using namespace Frenchie::Renderer;

// SceneView
SceneView::SceneView(const std::string& _Name, std::shared_ptr<Scene3D> _Scene3D) : Layer(_Name), m_Scene(_Scene3D){}

SceneView::~SceneView(){}

bool SceneView::awake() 
{
    m_CommandsQueue = Application::instance()->find<CommandsQueueLayer>();
    m_TimeProvider  = Application::instance()->find<TimeProviderLayer>();

    return m_CommandsQueue != nullptr && 
           m_TimeProvider  != nullptr && 
           m_Scene         != nullptr;
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

    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

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
        auto cursorNDCPosition = SceneView::to_ndc(
            size->get_size(),
            glm::vec3(cursorOpenGLPosition.x, cursorOpenGLPosition.y, +1.f)
        );

        auto viewportScaleMatrix     = m_Scene->get_viewport_scale_matrix();
        auto cameraViewMatrix        = camera->get_view_matrix();
        auto cameraProjectionMatrix  = camera->get_projection_matrix();
        auto inverseConversionMatrix = glm::inverse(cameraProjectionMatrix * cameraViewMatrix * viewportScaleMatrix);
        auto cursorWorldPosition     = inverseConversionMatrix * glm::vec4(cursorNDCPosition, 1.f);
        auto mouseTrackerText        = fmt::format("X : {}  Y : {}", cursorWorldPosition.x, cursorWorldPosition.y);

        ImGui::GetWindowDrawList()->AddText(
            ImVec2(mousePos.x, mousePos.y) - ImGui::CalcTextSize(mouseTrackerText.c_str()), 
            IM_COL32(255, 255, 0, 255), 
            mouseTrackerText.c_str()
        );

        m_Scene->set_cursor_position(cursorWorldPosition);
    }

    process_events();

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

glm::vec3 SceneView::to_ndc(const glm::vec2& _ScreenSize, const glm::vec3& _OpenGLPosition)
{
    return glm::vec3(
        ((float)_OpenGLPosition.x / (float)_ScreenSize.x  - 0.5f) * 2.0f,
        ((float)_OpenGLPosition.y / (float)_ScreenSize.y - 0.5f) * 2.0f,
        +1.0
    );
}

void SceneView::process_events()
{
    if(!ImGui::IsWindowHovered(
        ImGuiHoveredFlags_::ImGuiHoveredFlags_None)) 
        return;

    auto camera = m_Scene != nullptr ? m_Scene->get_component<Camera>() : nullptr;

    if(camera == nullptr) 
        return;

    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape) || 
        ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
    {
        for(auto&& item : m_Selection)
        {
            item.Object->set_flag(Object::Flags::Selected, false);
            item.Object->set_flag(Object::Flags::Focused, false);
        }

        m_Selection.clear();
    }

    // move items
    if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
    {
        auto mousePicker = m_Scene->get_component<Scene3DMousePicker>();
        m_Selection      = mousePicker->pick();
    }

    if(ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
    {
        for(auto&& item : m_Selection)
        {
            item.Object->set_flag(Object::Flags::Selected, true);
            item.Object->set_flag(Object::Flags::Focused, true);

            glm::vec3 delta = glm::vec3(ImGui::GetMouseDragDelta().x, -ImGui::GetMouseDragDelta().y, 0.f);

            item.Object->get_component<Transform>()->set_position(item.Position + 2.f * delta);
        }
    }

    // process input events
    m_Scene->apply_to_children_recursive(
        [this, &camera](Object* _Object)
        {
            auto meshRenderer = _Object->get_component<MeshRenderer>();
            auto transform    = _Object->get_component<Transform>();

            if(meshRenderer == nullptr || 
                    transform == nullptr) 
                return;

            // move object
            const double speed = camera->get_movement_speed();

            // const glm::vec3 perspectiveScale = 
            //     camera->get_object_perspective_scale(transform->get_model_matrix());

            // left mouse events
            // if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
            // {
            //     auto modelMatrix      = transform->get_model_matrix();
            //     auto scaleMatrix      = m_Scene->get_viewport_scale_matrix();
            //     auto viewMatrix       = camera->get_view_matrix();
            //     auto projectionMatrix = camera->get_projection_matrix();
            //     auto shaderMatrix     = projectionMatrix * viewMatrix * scaleMatrix * modelMatrix;

            //     auto aabb = meshRenderer->get_mesh()->get_aabb().transform(glm::scale(glm::mat4(1.f), glm::vec3(1.f / shaderMatrix[3][3])) * modelMatrix);

            //     if(aabb.intersects(ray))
            //     {
            //         _Object->set_flag(Object::Flags::Focused, true);

            //         m_Selection.insert(
            //             {
            //                 _Object, 
            //                 _Object->get_component<Transform>()->get_position()
            //             }
            //         );
            //     }
            // }

            // right mouse events
            if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Right))
            {
            }

            if(ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Right))
            {
            }
            
            if(ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Right))
            {
            }

            // middle mouse events
            if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Middle))
            {
            }
            
            if(ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Middle))
            {
            }
            
            if(ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Middle))
            {
            }

            // delete
            if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete))
            {
                if(_Object->check_flag(Object::Flags::Focused))
                {
                    m_CommandsQueue->push<CallbackCommand>(
                        [this]()
                        {
                            m_Scene->remove_children(
                                [](Object* _Object)->bool
                                {
                                    return _Object->check_flag(Object::Flags::Focused);
                                }
                            );
                        }
                    );
                }
            }

            // copy
            if((ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightCtrl)) && 
                ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_C))
            {
                // TODO: add logic here
            }

            // paste
            if((ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightCtrl)) && 
                ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_V))
            {
                // TODO: add logic here
            }
            
            if(_Object->check_flag(Object::Flags::Focused))
            {
                // move object left
                glm::vec3 movement = glm::vec3(0.f);
                
                if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftArrow))
                    movement = glm::vec3(-speed * m_TimeProvider->get_time_delta() / m_Scene->get_viewport_scale().x, 0.f, 0.f);

                if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightArrow))
                    movement = glm::vec3(+speed * m_TimeProvider->get_time_delta() / m_Scene->get_viewport_scale().x, 0.f, 0.f);

                if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) || 
                    ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightCtrl))
                {
                    if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_UpArrow))
                        movement = glm::vec3(0.f, 0.f, -speed * m_TimeProvider->get_time_delta() / m_Scene->get_viewport_scale().z);

                    if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_DownArrow))
                        movement = glm::vec3(0.f, 0.f, +speed * m_TimeProvider->get_time_delta() / m_Scene->get_viewport_scale().z);
                }
                else
                {
                    if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_UpArrow))
                        movement = glm::vec3(0.f, +speed * m_TimeProvider->get_time_delta() / m_Scene->get_viewport_scale().y, 0.f);

                    if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_DownArrow))
                        movement = glm::vec3(0.f, -speed * m_TimeProvider->get_time_delta() / m_Scene->get_viewport_scale().y, 0.f);
                }

                transform->set_position(transform->get_position() + movement);
            }
        }
    );
}