#include <FrenchieApplicationHierarchyViewLayer.hpp>

#include <FrenchieRendererTransform.hpp>

#include <FrenchieCoreLogger.hpp>

#include <imgui.h>

#include <string>

using namespace Frenchie::Application;
using namespace Frenchie::Renderer;

HierarchyView::HierarchyView(const std::string& _Name, Scene3D* _Scene3D) : Layer(_Name), m_Scene(_Scene3D){}
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
    DrawTree(m_Scene, id);

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

void HierarchyView::DrawTree(Hierarchy* _Transform, int& _ID)
{
    if(_Transform == nullptr || m_Scene == nullptr) 
        return;

    ImGui::PushID(_ID++);

    if (ImGui::TreeNodeEx((_Transform->is_selected() ? "" : _Transform->get_name()).c_str(),
        ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow   | 
        ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Framed        |
        ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DrawLinesFull | 
        ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen   | 
        ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_AllowOverlap))
    {
        if(ImGui::IsItemHovered() && 
            ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            m_Scene->set_selected(false);
            m_Scene->apply_to_children_recursive(
                [](Hierarchy* _Object)
                {
                    _Object->set_selected(false);
                }
            );

            _Transform->set_selected(true);
        }

        if (_Transform->is_selected())
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
                _Transform->set_selected(false);
                _Transform->set_focused(false);
                _Transform->set_name((newName.empty() ? "empty" : newName));
            }

            ImGui::PopID();
        }

        const auto& children = _Transform->get_children();

        for(auto&& child : children)
        {
            DrawTree(child, _ID);
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
}