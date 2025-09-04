#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationEditorMenuDrawer.hpp>
#include <FrenchieApplicationEditorCommandsQueueLayer.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class FileSystemExplorerFileMenu : public Layer
            {
            public:
                FileSystemExplorerFileMenu();
                virtual ~FileSystemExplorerFileMenu();

                // API
                FileSystemExplorer* get_caller() const;
                void draw(FileSystemExplorer*);

                // Frenchie::Application::Layer
                virtual bool allows_multiple_instances() const override;

            protected:
                FileSystemExplorer* m_Explorer = nullptr;
            };

            class FileMenuCopyAction : 
                public Frenchie::Application::Command::Registry<FileMenuCopyAction>
            {
            public:
                FileMenuCopyAction();
                virtual ~FileMenuCopyAction();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class FileMenuPasteAction : 
                public Frenchie::Application::Command::Registry<FileMenuPasteAction>
            {
            public:
                FileMenuPasteAction();
                virtual ~FileMenuPasteAction();

                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class FileMenuRemoveAction : 
                public Frenchie::Application::Command::Registry<FileMenuRemoveAction>
            {
            public:
                FileMenuRemoveAction();
                virtual ~FileMenuRemoveAction();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class FileMenuRenameAction : 
                public Frenchie::Application::Command::Registry<FileMenuRenameAction>
            {
            public:

                FileMenuRenameAction();
                virtual ~FileMenuRenameAction();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class FileMenuCreateFolderAction : 
                public Frenchie::Application::Command::Registry<FileMenuCreateFolderAction>
            {
            public:
                FileMenuCreateFolderAction();
                virtual ~FileMenuCreateFolderAction();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}