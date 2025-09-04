#include <FrenchieApplicationEditorFileSystemFilesRemoveDialogLayer.hpp>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

FilesRemoveDialog::FilesRemoveDialog(const std::set<std::filesystem::path>& _Paths) : 
    Dialog("Are you sure you want to delete these files ?"), 
    m_Paths(_Paths){}

FilesRemoveDialog::~FilesRemoveDialog(){}

void FilesRemoveDialog::draw_content()
{
    for(auto&& path : m_Paths)
        ImGui::TextUnformatted(path.string().c_str());
}

void FilesRemoveDialog::draw_buttons()
{
    if(ImGui::Button("Yes"))
    {
        if(m_Paths.empty())
            return;

        for(auto& path : m_Paths)
        {
            try
            {
                std::filesystem::remove_all(path);
            }
            catch(...)
            {
                Frenchie::Core::Logger::instance()->critical(fmt::format("Could not remove {}", path.string()));
            }
        }

        close();
    }
    
    ImGui::SameLine();
    
    if(ImGui::Button("No"))
        close();
}