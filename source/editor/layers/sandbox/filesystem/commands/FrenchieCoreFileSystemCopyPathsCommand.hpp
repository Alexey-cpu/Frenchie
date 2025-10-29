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
            class CopyPaths : public Frenchie::Application::Command::Registry<CopyPaths, const std::shared_ptr<Frenchie::Application::CommandPayload>&>
            {
            public:

                CopyPaths(const std::shared_ptr<Frenchie::Application::CommandPayload>&);
                virtual ~CopyPaths();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}