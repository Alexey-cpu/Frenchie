#include <FrenchieApplicationInspectorViewLayer.hpp>

#include <imgui.h>

#include <iostream>

using namespace Frenchie::Application;
using namespace Frenchie::Renderer;

// InspectorView
void InspectorView::SerachPanel::drawRegistryItem(const std::vector<std::string>& _Tree, int& _Index)
{
    (void)_Tree;
    (int)_Index;
}

void InspectorView::SerachPanel::draw(bool _Draw)
{
    if(_Draw)
        ImGui::OpenPopup("AddComponent");

    if (ImGui::BeginPopup("AddComponent"))
    {
        ImGui::InputText("Search", m_Search, 512);

        for(auto&& registry : Factory::registry())
        {
            // if(ImGui::BeginMenu("Menu"))
            // {
            //     if(ImGui::BeginMenu("Submenu"))
            //     {
            //         ImGui::EndMenu();
            //     }

            //     ImGui::EndMenu();
            // }

            // if(ImGui::Selectable(registry.first.c_str()))
            // {
            //     if(ImGui::BeginMenu("Menu"))
            //     {
            //         if(ImGui::BeginMenu("Submenu"))
            //         {
            //             ImGui::EndMenu();
            //         }

            //         ImGui::EndMenu();
            //     }

            //     // TODO: add a components creation logic here !!!
            // }
        }

        ImGui::EndPopup();
    }
}

// InspectorView
InspectorView::InspectorView(const std::string& _Name, std::shared_ptr<Scene3D> _Scene3D) : 
    Layer(_Name), 
    m_Scene(_Scene3D){}

InspectorView::~InspectorView(){}

bool InspectorView::awake()
{
    return Layer::awake();
}

void InspectorView::frame_start()
{
    return Layer::frame_start();
}

void InspectorView::frame_update()
{
    if(m_Scene == nullptr) 
        return;

    ImGui::Begin(get_name().c_str());

    if(m_Scene->check_flag(Object::Focused))
        m_Scene->draw_editor();

    m_Scene->apply_to_children_recursive(
        [](objectRef _Object)
        {
            if(_Object->check_flag(Object::Focused))
                _Object->draw_editor();
        }
    );

    auto avail = ImGui::GetContentRegionAvail();

    ImGui::SetCursorPos(ImVec2(avail.x * 0.25f, ImGui::GetCursorPos().y));

    m_SeachPanel.draw(ImGui::Button("AddComponent", ImVec2(avail.x * 0.5f, 0.f)));

    ImGui::End();

    Layer::frame_update();
}

void InspectorView::frame_finish()
{
    Layer::frame_finish();
}

void InspectorView::finish()
{
    Layer::finish();
}

void InspectorView::close()
{
    Layer::close();
}

bool InspectorView::is_closed()
{
    return Layer::is_closed();
}