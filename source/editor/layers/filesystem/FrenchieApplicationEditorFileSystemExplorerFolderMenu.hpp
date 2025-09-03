#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationCommandsQueueLayer.hpp>
#include <FrenchieApplicationEditorMenuDrawer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class FileSystemExplorerFolderMenu : public Layer
            {
            public:
                FileSystemExplorerFolderMenu();
                virtual ~FileSystemExplorerFolderMenu();

                // Frenchie::Application::Layer
                virtual void frame_update() override;

            protected:
                MenuDrawer m_MenuDrawer;  
            };

            class FolderMenuCreateFolderAction : 
                public Frenchie::Core::Command::Registry<FolderMenuCreateFolderAction>
            {
            public:

                FolderMenuCreateFolderAction();
                virtual ~FolderMenuCreateFolderAction();

                // Frenchie::Core::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class FolderMenuPasteAction : 
                public Frenchie::Core::Command::Registry<FolderMenuPasteAction>
            {
            public:
                FolderMenuPasteAction();
                virtual ~FolderMenuPasteAction();

                // Frenchie::Core::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}