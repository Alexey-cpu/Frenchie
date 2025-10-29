#pragma once

// Core
#include <FrenchieCoreStringUnicode.hpp>

// Application
#include <FrenchieApplicationCommandsLayer.hpp>

namespace Frenchie
{
    namespace Editor
    {
        namespace FileSystem
        {
            class PastePaths : public Frenchie::Application::Command::Registry<PastePaths, const std::shared_ptr<Frenchie::Application::CommandPayload>&>
            {
            public:

                PastePaths(const std::shared_ptr<Frenchie::Application::CommandPayload>&);
                virtual ~PastePaths();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}