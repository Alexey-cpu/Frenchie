#include <FrenchieApplicationEditorFileSystemFilesRenameDialogLayer.hpp>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

FilesRenameDialog::FilesRenameDialog(const std::set<std::filesystem::path>& _Paths): 
    Dialog("Rename files")
{
    for(auto&& path : _Paths)
    {
        auto textInput = std::make_shared<InputText>();
        textInput->set_buffer(Frenchie::Core::Helpers::String::as_utf8(path.filename().wstring()));
        m_Paths.insert({path, {textInput, true}});
    }
}

FilesRenameDialog::~FilesRenameDialog(){}

void FilesRenameDialog::draw_content()
{
    int checkboxID = 0;

    for(auto&& path : m_Paths)
    {
        ImGui::PushID(checkboxID++);
        ImGui::Checkbox("##", &path.second.second);
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::PushID(checkboxID++);
        path.second.first->draw("###");

        ImGui::PopID();
    }
}

void FilesRenameDialog::draw_buttons()
{
    if(ImGui::Button("Apply"))
    {
        // rename files
        for(auto& item : m_Paths)
        {
            auto source   = item.first;
            auto text     = item.second.first->get_buffer();
            auto selected = item.second.second;

            if(!std::filesystem::exists(source) || !selected) 
                continue;

            auto target = std::filesystem::path(
                source.parent_path().wstring()
                .append(L"/")
                .append(Frenchie::Core::Helpers::String::as_wide(text))
            );

            if(source == target) 
                continue;

            while (std::filesystem::exists(target))
            {
                auto extention = Frenchie::Core::Helpers::String::as_wide(
                    Frenchie::Core::Helpers::get_file_extention(target));

                target = source.parent_path().wstring()
                    .append(L"/")
                    .append(target.filename().stem().wstring())
                    .append(L"_Copy")
                    .append(extention);
            }
            
            try
            {
                std::filesystem::rename(source, target);
            }
            catch(const std::exception& e)
            {
                Frenchie::Core::Logger::instance()->critical(e.what());
            }
            
        }

        // close dialog
        close();
    }
    
    ImGui::SameLine();
    
    if(ImGui::Button("Cancel"))
        close();
}