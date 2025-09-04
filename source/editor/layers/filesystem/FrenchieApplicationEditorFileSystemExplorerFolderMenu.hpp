#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationEditorMenuDrawer.hpp>
#include <FrenchieApplicationCommandsQueueLayer.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

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

                // API
                FileSystemExplorer* get_caller() const;
                void draw(FileSystemExplorer*);

                // Frenchie::Application::Layer
                virtual bool allows_multiple_instances() const override;

            protected:
                FileSystemExplorer* m_Explorer = nullptr;
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