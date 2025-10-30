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
                namespace Selection{}

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
            class CopyFileCommand : public Frenchie::Application::Command::Registry<CopyFileCommand, const std::shared_ptr<Frenchie::Application::CommandPayload>&>
            {
            public:

                CopyFileCommand(const std::shared_ptr<Frenchie::Application::CommandPayload>&);
                virtual ~CopyFileCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class CopyFolderCommand : public Frenchie::Application::Command::Registry<CopyFolderCommand, const std::shared_ptr<Frenchie::Application::CommandPayload>&>
            {
            public:

                CopyFolderCommand(const std::shared_ptr<Frenchie::Application::CommandPayload>&);
                virtual ~CopyFolderCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class CopySelectionCommand : public Frenchie::Application::Command::Registry<CopySelectionCommand, const std::shared_ptr<Frenchie::Application::CommandPayload>&>
            {
            public:

                CopySelectionCommand(const std::shared_ptr<Frenchie::Application::CommandPayload>&);
                virtual ~CopySelectionCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}