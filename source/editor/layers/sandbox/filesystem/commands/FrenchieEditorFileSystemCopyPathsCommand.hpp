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

            class CopyFileCommand : public Command::Registry<CopyFileCommand, const CommandPayloads&>
            {
            public:

                CopyFileCommand(const CommandPayloads&);
                virtual ~CopyFileCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };

            class CopyFolderCommand : public Command::Registry<CopyFolderCommand, const CommandPayloads&>
            {
            public:

                CopyFolderCommand(const CommandPayloads&);
                virtual ~CopyFolderCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}