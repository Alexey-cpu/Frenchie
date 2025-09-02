#pragma once

#include <FrenchieApplicationEditorDialogLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            // FileRenamerDialog
            class FilesRemoveDialog : public Dialog
            {
            public:
                FilesRemoveDialog(const std::vector<std::filesystem::path>& _Paths);
                virtual ~FilesRemoveDialog();

                // Frenchie::Application::Editor::Dialog
                virtual void draw_content() override;
                virtual void draw_buttons() override;

            protected:
                std::vector<std::filesystem::path> m_Paths;
            };
        }
    }
}