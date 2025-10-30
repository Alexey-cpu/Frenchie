#pragma once

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

            class CreateFolderCommand :
                public Command::Registry<CreateFolderCommand, const CommandPayloads&>
            {
            public:

                CreateFolderCommand(const CommandPayloads&);
                virtual ~CreateFolderCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}