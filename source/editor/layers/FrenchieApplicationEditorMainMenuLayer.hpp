#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationEditorMenu.hpp>
#include <FrenchieApplicationEditorCommandsQueueLayer.hpp>

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
                Menu m_MenuDrawer;
            };

            class MainMenuFrenchieMenuExitCommand : 
                public Frenchie::Application::Command::Registry<MainMenuFrenchieMenuExitCommand>
            {
            public:

                MainMenuFrenchieMenuExitCommand();
                virtual ~MainMenuFrenchieMenuExitCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            // MainMenuWindowsMenuConsoleCommand
            class MainMenuWindowsMenuConsoleCommand : 
                public Frenchie::Application::Command::Registry<MainMenuWindowsMenuConsoleCommand>
            {
            public:

                MainMenuWindowsMenuConsoleCommand();
                virtual ~MainMenuWindowsMenuConsoleCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            // MainMenuWindowsMenuFileSystemCommand
            class MainMenuWindowsMenuFileSystemCommand : 
                public Frenchie::Application::Command::Registry<MainMenuWindowsMenuFileSystemCommand>
            {
            public:

                MainMenuWindowsMenuFileSystemCommand();
                virtual ~MainMenuWindowsMenuFileSystemCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            // MainMenuWindowsMenuSettingsCommand
            class MainMenuWindowsMenuPreferencesCommand : 
                public Frenchie::Application::Command::Registry<MainMenuWindowsMenuPreferencesCommand>
            {
            public:

                MainMenuWindowsMenuPreferencesCommand();
                virtual ~MainMenuWindowsMenuPreferencesCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}