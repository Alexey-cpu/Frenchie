#include <FrenchieApplicationHierarchyViewLayer.hpp>

#include <FrenchieRendererTransform.hpp>

#include <FrenchieCoreLogger.hpp>

#include <imgui.h>

#include <string>

#include <iostream>

using namespace Frenchie::Application;
using namespace Frenchie::Renderer;

HierarchyView::HierarchyView(const std::string& _Name, std::shared_ptr<Scene3D> _Scene3D) : Layer(_Name), m_Scene(_Scene3D){}
HierarchyView::~HierarchyView(){}

bool HierarchyView::awake()
{
    return Layer::awake() && m_Scene != nullptr;
}

void HierarchyView::frame_start()
{
    Layer::frame_start();
}

void HierarchyView::frame_update()
{
    ImGui::Begin(get_name().c_str());

    int id = 0;
    DrawTree(m_Scene.get(), id);

    ImGui::End();
}

void HierarchyView::frame_finish()
{
    Layer::frame_finish();
}

void HierarchyView::finish()
{
    Layer::finish();
}

void HierarchyView::close()
{
    Layer::close();
}

bool HierarchyView::is_closed()
{
    return Layer::is_closed();
}

void HierarchyView::DrawTree(Object* _Transform, int& _ID)
{
    if(_Transform == nullptr || m_Scene == nullptr) 
        return;

    ImGui::PushID(_ID++);

    if (ImGui::TreeNodeEx((_Transform->check_flag(Object::Flags::Selected) ? "" : _Transform->get_name()).c_str(),
        ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow   | 
        ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Framed        |
        ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DrawLinesFull | 
        ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen   | 
        ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_AllowOverlap))
    {
        // select item on double click
        if(ImGui::IsItemHovered() && 
            ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            m_Scene->set_flag(Object::Flags::Selected, false);
            m_Scene->apply_to_children_recursive([](Object* _Object){_Object->set_flag(Object::Flags::Selected, false);});
            _Transform->set_flag(Object::Flags::Selected, true);
        }

        // rename selected item
        if (_Transform->check_flag(Object::Flags::Selected))
        {
            ImGui::SameLine();
            ImGui::PushID(_ID++);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

            std::string name = _Transform->get_name();

            for(int i = 0; i < 512; i++) 
            {
                if(i < name.size())
                    m_TextInput[i] = name[i];
                else 
                    m_TextInput[i] = '\0';
            }

            if(ImGui::InputText("", m_TextInput, 512, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
            {
                auto newName = std::string(m_TextInput);
                _Transform->set_flag(Object::Flags::Selected, false);
                _Transform->set_name((newName.empty() ? "empty" : newName));
            }

            ImGui::PopID();
        }

        // focus item on a single click
        if(ImGui::IsItemHovered() && 
            ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left)  || 
            ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Right) || 
            ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Middle))
        {
            m_Scene->set_flag(Object::Flags::Focused, false);
            m_Scene->apply_to_children_recursive([](Object* _Object){_Object->set_flag(Object::Flags::Focused, false);});
            _Transform->set_flag(Object::Flags::Focused, true);
        }

        // recursivelly draw children
        const auto& children = _Transform->get_children();

        for(auto&& child : children) 
            DrawTree(child.get(), _ID);
        ImGui::TreePop();
    }

    ImGui::PopID();

    // deselect all items on click
    Object* selectedItem = m_Scene->check_flag(Object::Flags::Selected) ? m_Scene.get() : nullptr;
    Object* focusedItem  = m_Scene->check_flag(Object::Flags::Selected) ? m_Scene.get() : nullptr;

    m_Scene->apply_to_children_recursive(
        [&selectedItem, &focusedItem](Object* _Object)
        {
            if(_Object->check_flag(Object::Flags::Selected)) 
                selectedItem = _Object;

            if(_Object->check_flag(Object::Flags::Focused)) 
                focusedItem = _Object;
        }
    );

    if(focusedItem != nullptr && 
        focusedItem != selectedItem)
    {
        m_Scene->set_flag(Object::Flags::Selected, false);
        m_Scene->apply_to_children_recursive(
            [](Object* _Object)
            {
                _Object->set_flag(Object::Flags::Selected, false);
            }
        );
    }
}