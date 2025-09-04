#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationEditorMenu.hpp>
#include <FrenchieApplicationEditorCommandsQueueLayer.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class FileSystemExplorerFolderMenu
            {
            public:
                FileSystemExplorerFolderMenu();
                virtual ~FileSystemExplorerFolderMenu();

                // API
                void draw(FileSystemExplorer*);
            };

            class FolderMenuCreateFolderAction : 
                public Frenchie::Application::Command::Registry<FolderMenuCreateFolderAction, void*>
            {
            public:

                FolderMenuCreateFolderAction(void* _Sender);
                virtual ~FolderMenuCreateFolderAction();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class FolderMenuPasteAction : 
                public Frenchie::Application::Command::Registry<FolderMenuPasteAction, void*>
            {
            public:
                FolderMenuPasteAction(void* _Sender);
                virtual ~FolderMenuPasteAction();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}