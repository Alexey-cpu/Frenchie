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

            class PastePathsCommand : public Command::Registry<PastePathsCommand, const CommandPayloads&>
            {
            public:

                PastePathsCommand(const CommandPayloads&);
                virtual ~PastePathsCommand();

                // Frenchie::Application::Command
                virtual void execute() override;

                // Command::TRegistryType
                static std::string factory_id();
            };
        }
    }
}