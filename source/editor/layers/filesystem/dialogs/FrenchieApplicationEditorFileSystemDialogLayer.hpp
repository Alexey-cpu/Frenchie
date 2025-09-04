#pragma once

#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class FilesOpenDialog : public FileSystemExplorer 
            {
            public:
                FilesOpenDialog(
                    const std::function<void()>& _OnAccpected, 
                    const std::string&           _Name = STRINGIFY(FilesOpenDialog));
                
                virtual ~FilesOpenDialog();

                // Frenchie::Application::Layer
                virtual void frame_update() override;
                virtual bool allows_multiple_instances() const override;

            protected:
                std::function<void()> m_OnAccepted = nullptr;
            };
        }
    }
}