#pragma once

// Core
#include <FrenchieCoreStringUnicode.hpp>

// Application
#include <FrenchieApplication.hpp>

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

            class RemoveFolderCommand :
                public Command::Registry<RemoveFolderCommand, const CommandPayloads&>
            {
            public:

                RemoveFolderCommand(const CommandPayloads&);
                virtual ~RemoveFolderCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class RemoveFileCommand :
                public Command::Registry<RemoveFileCommand, const CommandPayloads&>
            {
            public:

                RemoveFileCommand(const CommandPayloads&);
                virtual ~RemoveFileCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}