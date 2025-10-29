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
            class CreateFolderCommand : public Frenchie::Application::Command::Registry<CreateFolderCommand, void*>
            {
            public:

                CreateFolderCommand(void*);
                virtual ~CreateFolderCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}