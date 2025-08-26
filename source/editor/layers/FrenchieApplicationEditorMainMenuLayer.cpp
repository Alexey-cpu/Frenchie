#include <FrenchieApplicationEditorMainMenuLayer.hpp>

#include <FrenchieApplication.hpp>
#include <FrenchieApplicationEditorConsoleLayer.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>
#include <FrenchieApplicationCommandsQueueLayer.hpp>

#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreCommand.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application::Editor;

// IMGUI
#include <imgui.h>
#include <imgui_internal.h>

#include <set>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            // Frenchie menu
            class MainMenuFrenchieMenuExitCommand : 
                public Frenchie::Core::Command::Registry<MainMenuFrenchieMenuExitCommand>
            {
            public:

                // Frenchie::Core::Command
                virtual void execute() override
                {
                    Frenchie::Application::Application::instance()->close();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::MainMenu), "Frenchie::Exit");
                }
            };

            // windows menu
            class MainMenuWindowsMenuConsoleCommand : 
                public Frenchie::Core::Command::Registry<MainMenuWindowsMenuConsoleCommand>
            {
            public:

                // Frenchie::Core::Command
                virtual void execute() override
                {
                    Frenchie::Application::Application::instance()->find_or_push<Console>()->show();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::MainMenu), "Windows::Console");
                }
            };

            // windows menu
            class MainMenuWindowsMenuFileSystemCommand : 
                public Frenchie::Core::Command::Registry<MainMenuWindowsMenuFileSystemCommand>
            {
            public:

                // Frenchie::Core::Command
                virtual void execute() override
                {
                    Frenchie::Application::Application::instance()->find_or_push<FileSystem::Explorer>()->show();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::MainMenu), "Windows::FileSystem");
                }
            };
        }
    }
}

// Menu
MainMenu::MainMenu() : Layer(STRINGIFY(MainMenu)){}

MainMenu::~MainMenu(){}

void MainMenu::frame_update()
{
    if(ImGui::BeginMainMenuBar())
    {
        m_MenuDrawer.draw(STRINGIFY(Frenchie::Application::Editor::MainMenu));
        ImGui::EndMainMenuBar();
    }
}