#pragma once

#include <FrenchieApplicationEditorDialogLayer.hpp>
#include <FrenchieApplicationEditorInputText.hpp>

// STL
#include <set>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            // FileRenamerDialog
            class FilesRenameDialog : public Dialog
            {
            public:

                FilesRenameDialog(const std::set<std::filesystem::path>& _Paths);
                virtual ~FilesRenameDialog();

                // Frenchie::Application::Editor::Dialog
                virtual void draw_content() override;
                virtual void draw_buttons() override;

            protected:
                std::map<
                    std::filesystem::path, 
                    std::pair<std::shared_ptr<InputText>, bool>> m_Paths;
            };
        }
    }
}