#include <FrenchieEditorFileSystemExplorerDialog.hpp>

using namespace Frenchie::Application;
using namespace Frenchie::Application::Configuration;

using namespace Frenchie::Editor::FileSystem;

// OpenFilesDialog
ExplorerDialog::ExplorerDialog(const std::string& _Name): 
    FileSystem::Explorer(_Name, std::filesystem::current_path()){}

ExplorerDialog::~ExplorerDialog(){}

void ExplorerDialog::frame_update()
{
    ImGui::OpenPopup(m_Name.c_str());

    ImGuiStyle& style = ImGui::GetStyle();

    // press button
    auto wiondowFlags =
        ImGuiWindowFlags_::ImGuiWindowFlags_None        | 
        ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | 
        ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::SetNextWindowSizeConstraints(ImVec2(512, 512), ImVec2(FLT_MAX, FLT_MAX));

    if(ImGui::BeginPopupModal(m_Name.c_str(), &m_Opened))
    {
        ImGui::BeginChild(
            "Content",
            ImVec2(
                ImGui::GetContentRegionAvail().x, 
                ImGui::GetContentRegionAvail().y - (ImGui::CalcTextSize("Button").y + style.FramePadding.x * 2.0f + ImGui::CalcTextSize("BUTTON").y)),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            wiondowFlags);
        {
            draw_contents();
            ImGui::EndChild();
        }

        ImGui::BeginChild(
            "Buttons",
            ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            wiondowFlags);

        {
            if(ImGui::Button(translator()->translate("Ok").c_str()))
            {
                if(m_OnAccepted != nullptr) 
                    m_OnAccepted();
                close();
            }

             ImGui::SameLine();

            if(ImGui::Button(translator()->translate("Cancel").c_str()))
                close();

            ImGui::EndChild();
        }

        ImGui::EndPopup();
    }
}

bool ExplorerDialog::allows_multiple_instances() const
{
    return false;
}

void ExplorerDialog::on_accepted(const std::function<void()>& _Callback)
{
    m_OnAccepted = _Callback;
}