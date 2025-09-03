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
            class FileSystemExplorerFileMenu : public Layer
            {
            public:
                FileSystemExplorerFileMenu();
                virtual ~FileSystemExplorerFileMenu();

                // Layer
                virtual void frame_update() override;

            protected:
                MenuDrawer m_MenuDrawer;
            };

            class FileMenuCopyAction : 
                public Frenchie::Core::Command::Registry<FileMenuCopyAction>
            {
            public:
                FileMenuCopyAction();
                virtual ~FileMenuCopyAction();

                // Frenchie::Core::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class FileMenuPasteAction : 
                public Frenchie::Core::Command::Registry<FileMenuPasteAction>
            {
            public:
                FileMenuPasteAction();
                virtual ~FileMenuPasteAction();

                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class FileMenuRemoveAction : 
                public Frenchie::Core::Command::Registry<FileMenuRemoveAction>
            {
            public:
                FileMenuRemoveAction();
                virtual ~FileMenuRemoveAction();

                // Frenchie::Core::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class FileMenuRenameAction : 
                public Frenchie::Core::Command::Registry<FileMenuRenameAction>
            {
            public:

                FileMenuRenameAction();
                virtual ~FileMenuRenameAction();

                // Frenchie::Core::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class FileMenuCreateFolderAction : 
                public Frenchie::Core::Command::Registry<FileMenuCreateFolderAction>
            {
            public:
                FileMenuCreateFolderAction();
                virtual ~FileMenuCreateFolderAction();

                // Frenchie::Core::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}