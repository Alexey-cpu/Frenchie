#include <FrenchieApplicationEditorAbstractDialogLayer.hpp>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Application;
using namespace Frenchie::Editor;

Dialog::Dialog(const std::string& _Name, const float& _MinWidth, const float& _MinHeight) : 
    Layer(_Name), 
    m_MinWidth(_MinWidth), 
    m_MinHeight(_MinHeight){}

Dialog::~Dialog(){}

void Dialog::frame_update()
{   
    ImGui::OpenPopup(m_Name.c_str());

    ImGuiStyle& style = ImGui::GetStyle();

    ImGui::SetNextWindowSizeConstraints(ImVec2(m_MinWidth, m_MinHeight), ImVec2(FLT_MAX, FLT_MAX));

    // press button
    auto wiondowFlags =
        ImGuiWindowFlags_::ImGuiWindowFlags_None        | 
        ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | 
        ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse;

    if(ImGui::BeginPopupModal(m_Name.c_str(), &m_Opened))
    {
        ImGui::BeginChild(
            "Content",
            ImVec2(
                ImGui::GetContentRegionAvail().x, 
                ImGui::GetContentRegionAvail().y - (ImGui::CalcTextSize("Button").y + style.FramePadding.x * 2.0f + ImGui::CalcTextSize("BUTTON").y)),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            wiondowFlags);
        draw_content();
        ImGui::EndChild();

        ImGui::BeginChild(
            "Buttons",
            ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            wiondowFlags);
        draw_buttons();
        ImGui::EndChild();

        ImGui::EndPopup();
    }
}

bool Dialog::allows_multiple_instances() const
{
    return false;
}