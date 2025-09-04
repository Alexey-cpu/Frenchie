#pragma once

#include <FrenchieApplicationEditorDialogLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class FilesOpenDialog : public Dialog 
            {
            public:
                FilesOpenDialog(
                    const std::filesystem::path& = std::filesystem::current_path(), 
                    const std::string&           = STRINGIFY(FilesOpenDialog));
                
                virtual ~FilesOpenDialog();

                // Frenchie::Application::Editor::Dialog
                virtual void draw_content() override;
                virtual void draw_buttons() override;
            };
        }
    }
}