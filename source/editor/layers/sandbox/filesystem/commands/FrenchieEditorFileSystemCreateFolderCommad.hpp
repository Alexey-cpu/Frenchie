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
            namespace Menu
            {
                namespace Folder{}

                namespace File{}
            }
        }
    }
}

namespace Frenchie
{
    namespace Editor
    {
        namespace FileSystem
        {
            class CreateFolderCommand :
                public Frenchie::Application::Command::Registry<CreateFolderCommand, const std::shared_ptr<Frenchie::Application::CommandPayload>&>
            {
            public:

                CreateFolderCommand(const std::shared_ptr<Frenchie::Application::CommandPayload>&);
                virtual ~CreateFolderCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}