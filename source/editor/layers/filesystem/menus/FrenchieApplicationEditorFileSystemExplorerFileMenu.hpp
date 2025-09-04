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
            class FileSystemExplorerFileMenu
            {
            public:
                FileSystemExplorerFileMenu();
                virtual ~FileSystemExplorerFileMenu();

                // API
                void draw(FileSystemExplorer*);
            };

            class FileMenuCopyAction : 
                public Frenchie::Application::Command::Registry<FileMenuCopyAction, void*>
            {
            public:
                FileMenuCopyAction(void* _Sender = nullptr);
                virtual ~FileMenuCopyAction();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class FileMenuPasteAction : 
                public Frenchie::Application::Command::Registry<FileMenuPasteAction, void*>
            {
            public:
                FileMenuPasteAction(void* _Sender = nullptr);
                virtual ~FileMenuPasteAction();

                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class FileMenuRemoveAction : 
                public Frenchie::Application::Command::Registry<FileMenuRemoveAction, void*>
            {
            public:
                FileMenuRemoveAction(void* _Sender = nullptr);
                virtual ~FileMenuRemoveAction();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class FileMenuRenameAction : 
                public Frenchie::Application::Command::Registry<FileMenuRenameAction, void*>
            {
            public:

                FileMenuRenameAction(void* _Sender = nullptr);
                virtual ~FileMenuRenameAction();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class FileMenuCreateFolderAction : 
                public Frenchie::Application::Command::Registry<FileMenuCreateFolderAction, void*>
            {
            public:
                FileMenuCreateFolderAction(void* _Sender = nullptr);
                virtual ~FileMenuCreateFolderAction();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}