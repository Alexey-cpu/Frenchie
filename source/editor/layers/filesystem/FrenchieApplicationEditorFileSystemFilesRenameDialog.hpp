#pragma once

#include <FrenchieApplicationEditorDialogLayer.hpp>
#include <FrenchieApplicationEditorInputTextDrawer.hpp>

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

                FilesRenameDialog(const std::vector<std::filesystem::path>& _Paths);
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