#pragma once

#pragma once

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsQueueLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
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