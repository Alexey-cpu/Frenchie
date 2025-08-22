#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
        }
    }
}

FlileSystemExplorer::FlileSystemExplorer() : Layer(STRINGIFY(FlileSystemExplorer))
{
}

FlileSystemExplorer::~FlileSystemExplorer(){}

void FlileSystemExplorer::frame_update()
{
    // collect files info
    for(const auto& directory :
        std::filesystem::directory_iterator(std::filesystem::current_path().make_preferred(), 
        std::filesystem::directory_options::skip_permission_denied))
    {
        auto extention = Frenchie::Core::Helpers::get_file_extention(directory.path());

        m_ForamtFilter[extention] = 
            m_ForamtFilter.find(extention) == m_ForamtFilter.end() ? true : m_ForamtFilter[extention];
    }

    // draw
    ImGui::Begin(get_name().c_str());
    {
        // draw current path editor
        if(m_CurrentDirectory.draw(
            Frenchie::Core::Helpers::String::as_utf8(std::filesystem::current_path().make_preferred().wstring()).c_str(), 
            ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
        {
            std::filesystem::current_path(std::filesystem::path(m_CurrentDirectory.get_buffer()).make_preferred());
        }

        // draw table
        if (ImGui::BeginTable(
                "CurrentDirectoryContentTable",
                3,
                ImGuiTableFlags_::ImGuiTableFlags_ScrollY      | 
                ImGuiTableFlags_::ImGuiTableFlags_RowBg        | 
                ImGuiTableFlags_::ImGuiTableFlags_BordersOuter | 
                ImGuiTableFlags_::ImGuiTableFlags_BordersV     |
                ImGuiTableFlags_::ImGuiTableFlags_Resizable    |
                ImGuiTableFlags_::ImGuiTableFlags_Reorderable  |
                ImGuiTableFlags_::ImGuiTableFlags_Hideable, 
                ImVec2(0.0, ImGui::GetContentRegionAvail().y - 2.0f * ImGui::GetTextLineHeightWithSpacing())))
        {
            // setup columns
            ImGui::TableSetupColumn("name", 
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch |
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);
            
            ImGui::TableSetupColumn("last write time", 
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthFixed |
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);

            ImGui::TableSetupColumn("type", 
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
            
            ImGui::TableHeadersRow();

            // draw content of current directory
            for(const auto& directory :
                std::filesystem::directory_iterator(std::filesystem::current_path().make_preferred(), 
                std::filesystem::directory_options::skip_permission_denied))
            {
                if(m_ForamtFilter.find(Frenchie::Core::Helpers::get_file_extention(directory.path())) == m_ForamtFilter.end()) 
                    continue;

                ImGui::TableNextRow();

                // draw name
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(directory.path().filename().string().c_str());

                // draw last write time
                ImGui::TableSetColumnIndex(1);
                auto time    = std::filesystem::last_write_time(directory);
                auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
                ImGui::TextUnformatted(asctime(std::localtime(&seconds)));

                // draw type
                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted((directory.is_directory() ? "folder" : "file"));
            }

            ImGui::EndTable();
        }

        // TODO: draw format filter
        // if(ImGui::BeginCombo("format filter", "filter"))
        // {
        //     ImGui::TextUnformatted("Item-1");
        //     ImGui::TextUnformatted("Item-2");
        //     ImGui::TextUnformatted("Item-3");
        //     ImGui::TextUnformatted("Item-4");
        //     ImGui::TextUnformatted("Item-5");

        //     ImGui::EndCombo();
        // }

        ImGui::End();
    }
}