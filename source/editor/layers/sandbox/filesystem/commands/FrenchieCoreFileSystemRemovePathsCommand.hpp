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
            class RemoveFolderCommand :
                public Frenchie::Application::Command::Registry<RemoveFolderCommand, const std::shared_ptr<Frenchie::Application::CommandPayload>&>
            {
            public:

                RemoveFolderCommand(const std::shared_ptr<Frenchie::Application::CommandPayload>&);
                virtual ~RemoveFolderCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class RemoveFileCommand :
                public Frenchie::Application::Command::Registry<RemoveFileCommand, const std::shared_ptr<Frenchie::Application::CommandPayload>&>
            {
            public:

                RemoveFileCommand(const std::shared_ptr<Frenchie::Application::CommandPayload>&);
                virtual ~RemoveFileCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}