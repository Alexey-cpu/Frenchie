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
            using namespace Frenchie::Application;

            class RenameFolderCommand :
                public Command::Registry<RenameFolderCommand, const CommandPayloads&>
            {
            public:

                RenameFolderCommand(const CommandPayloads&);
                virtual ~RenameFolderCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class RenameFileCommand :
                public Command::Registry<RenameFileCommand, const CommandPayloads&>
            {
            public:

                RenameFileCommand(const CommandPayloads&);
                virtual ~RenameFileCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}