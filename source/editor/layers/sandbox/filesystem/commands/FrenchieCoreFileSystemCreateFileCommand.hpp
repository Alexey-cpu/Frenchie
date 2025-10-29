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
            class CreateFileCommand : public Frenchie::Application::Command::Registry<CreateFileCommand, const std::shared_ptr<Frenchie::Application::CommandPayload>&>
            {
            public:

                CreateFileCommand(const std::shared_ptr<Frenchie::Application::CommandPayload>&);
                virtual ~CreateFileCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}