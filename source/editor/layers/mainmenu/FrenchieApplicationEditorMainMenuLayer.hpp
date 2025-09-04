#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationEditorMenuDrawer.hpp>

// Core
#include <FrenchieCoreCommand.hpp>

// STL
#include <map>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class MainMenu : public Layer
            {
            public:
                MainMenu();
                virtual ~MainMenu();

                // Frenchie::Application::Layer
                virtual void frame_update() override;

            protected:
                MenuDrawer m_MenuDrawer;
            };

            class MainMenuFrenchieMenuExitCommand : 
                public Frenchie::Core::Command::Registry<MainMenuFrenchieMenuExitCommand>
            {
            public:

                MainMenuFrenchieMenuExitCommand();
                virtual ~MainMenuFrenchieMenuExitCommand();

                // Frenchie::Core::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            // MainMenuWindowsMenuConsoleCommand
            class MainMenuWindowsMenuConsoleCommand : 
                public Frenchie::Core::Command::Registry<MainMenuWindowsMenuConsoleCommand>
            {
            public:

                MainMenuWindowsMenuConsoleCommand();
                virtual ~MainMenuWindowsMenuConsoleCommand();

                // Frenchie::Core::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            // MainMenuWindowsMenuFileSystemCommand
            class MainMenuWindowsMenuFileSystemCommand : 
                public Frenchie::Core::Command::Registry<MainMenuWindowsMenuFileSystemCommand>
            {
            public:

                MainMenuWindowsMenuFileSystemCommand();
                virtual ~MainMenuWindowsMenuFileSystemCommand();

                // Frenchie::Core::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            // MainMenuWindowsMenuSettingsCommand
            class MainMenuWindowsMenuPreferencesCommand : 
                public Frenchie::Core::Command::Registry<MainMenuWindowsMenuPreferencesCommand>
            {
            public:

                MainMenuWindowsMenuPreferencesCommand();
                virtual ~MainMenuWindowsMenuPreferencesCommand();

                // Frenchie::Core::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}