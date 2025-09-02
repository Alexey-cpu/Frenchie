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
            // Frenchie menu
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
        }
    }
}