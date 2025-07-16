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
    if(m_Scene == nullptr) 
        return;

    auto camera   = m_Scene->get_component<Camera>();
    auto size     = m_Scene->get_component<Size>();
    auto renderer = m_Scene->get_component<IRenderer>();

    if(camera == nullptr || size == nullptr || renderer == nullptr)
        return;

    ImGui::Begin(get_name().c_str());

    // draw scene contents and update scene geometry
    ImVec2 sceneWidgetPosition = ImGui::GetCursorScreenPos();
    float  sceneTextureHeight  = ImGui::GetContentRegionAvail().y;
    float  sceneTextureWidth   = sceneTextureHeight * size->get_aspect();
    
    if(sceneTextureWidth < ImGui::GetContentRegionAvail().x)
    {
        sceneTextureWidth  = ImGui::GetContentRegionAvail().x;
        sceneTextureHeight = sceneTextureWidth / size->get_aspect();
    }

    ImGui::GetWindowDrawList()->AddImage(
        renderer->get_texture(), 
        ImVec2(sceneWidgetPosition.x, sceneWidgetPosition.y), 
        ImVec2(sceneWidgetPosition.x + sceneTextureWidth, sceneWidgetPosition.y + sceneTextureHeight), 
        ImVec2(0, 1), // in ImGUI UV coordinates are flipped
        ImVec2(1, 0)
    );

    // draw scene content bounding rectangle and cast mouse cursor ray
    ImRect sceneViewportRect     = ImRect(ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos(), ImGui::GetWindowContentRegionMax() + ImGui::GetWindowPos());
    ImVec2 mousePosition         = ImGui::GetMousePos();
    ImVec2 windowContentPosition = sceneViewportRect.GetTL();
    
    glm::vec3 cursorOpenGLPosition = glm::vec3(
        mousePosition.x - windowContentPosition.x, 
        sceneTextureHeight - mousePosition.y + windowContentPosition.y - 1, 
        0.f
    );

    //-------------------------------------------------------------------------------------------------
    //
    //-------------------------------------------------------------------------------------------------
    m_SceneCursor.PreviousPosition = m_SceneCursor.CurrentPosition;
    m_SceneCursor.CurrentPosition  = cursorOpenGLPosition;
    m_SceneCursor.PositionDelta            = m_SceneCursor.CurrentPosition - m_SceneCursor.PreviousPosition;
    //-------------------------------------------------------------------------------------------------

    // compute cursor scene (world) position
    process_events(SceneView::to_ndc(
        glm::vec2(sceneTextureWidth, sceneTextureHeight),
        glm::vec3(cursorOpenGLPosition.x, cursorOpenGLPosition.y, +1.f)));

    ImGui::End();

    m_Scene->frame_update();
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

void SceneView::process_events(const glm::vec3& _CursorNDCPosition)
{
    if(!ImGui::IsWindowHovered(
        ImGuiHoveredFlags_::ImGuiHoveredFlags_None) || 
        m_Scene == nullptr) 
        return;

    auto camera      = m_Scene->get_component<Camera>();
    auto mousePicker = m_Scene->get_component<Scene3DMousePicker>();

    if(camera == nullptr || mousePicker == nullptr) 
        return;

    // clear selection
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

    // pick objects
    if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
    {
        m_Selection = mousePicker->pick(_CursorNDCPosition);
    }

    // drag objects
    if(ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
    {
        auto projectionMatrix = 
            camera->get_projection_matrix() * camera->get_view_matrix();

        for(auto&& item : m_Selection)
        {
            item.Object->set_flag(Object::Flags::Focused, true);

            // scale and rotate mouse cursor movement
            glm::vec3 perspectiveScale = glm::vec3((projectionMatrix * item.Object->get_component<Transform>()->get_model_matrix())[3][3]);
            glm::vec3 mainViewportSize = glm::vec3(ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y, 1.f);
            glm::vec3 thisWindowSize   = glm::vec3(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y, 1.f);
            
            auto mouseDelta = 
                m_SceneCursor.PositionDelta / std::max(thisWindowSize.x, thisWindowSize.y) * std::max(mainViewportSize.x, mainViewportSize.y) * perspectiveScale;

            mouseDelta = 
                glm::rotate(glm::mat4(1.f), glm::radians(camera->get_pitch()), glm::vec3(1.f, 0.f, 0.f)) * 
                glm::rotate(glm::mat4(1.f), glm::radians(camera->get_yaw()), glm::vec3(0.f, 1.f, 0.f)) * 
                glm::rotate(glm::mat4(1.f), glm::radians(camera->get_roll()), glm::vec3(0.f, 0.f, 1.f)) * 
                glm::vec4(mouseDelta, 1.f);

            item.Object->get_component<Transform>()->set_position(
                item.Object->get_component<Transform>()->get_position() + mouseDelta);

            break; // move only the nearest to the cursor element
        }
    }

    // move camera
    if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightCtrl))
    {
        // set camera rotation
        camera->set_pitch(camera->get_pitch() + m_SceneCursor.PositionDelta.y * camera->get_sensitivity());
        camera->set_yaw(camera->get_yaw() - m_SceneCursor.PositionDelta.x * camera->get_sensitivity());

        // set camera position
        auto path     = camera->get_movement_speed() * (float)m_TimeProvider->get_time_delta();
        auto position = camera->get_position() * m_Scene->get_component<Transform>()->get_scale();

        if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_UpArrow) || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_W))
            position += path * camera->get_front();

        if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_DownArrow) || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_S))
            position -= path * camera->get_front();

        if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftArrow) ||  ImGui::IsKeyDown(ImGuiKey::ImGuiKey_A))
            position -= path * glm::normalize(glm::cross(camera->get_front(), camera->get_up()));

        if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightArrow) || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_D))
            position += path * glm::normalize(glm::cross(camera->get_front(), camera->get_up()));

        camera->set_position(position / m_Scene->get_component<Transform>()->get_scale());
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

            // delete objects
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
            
            // move objects
            if(_Object->check_flag(Object::Flags::Focused))
            {
                auto path     = camera->get_movement_speed() * (float)m_TimeProvider->get_time_delta();
                auto position = transform->get_position();
                
                if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftArrow))
                    position += path * glm::vec3(-1.f, 0.f, 0.f) / m_Scene->get_component<Transform>()->get_scale();

                if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightArrow))
                    position += path * glm::vec3(+1.f, 0.f, 0.f) / m_Scene->get_component<Transform>()->get_scale();

                if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_UpArrow))
                    position += path * glm::vec3(0.f, +1, 0.f) / m_Scene->get_component<Transform>()->get_scale();

                if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_DownArrow))
                    position += path * glm::vec3(0.f, -1, 0.f) / m_Scene->get_component<Transform>()->get_scale();

                transform->set_position(position);
            }
        }
    );
}