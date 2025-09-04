#include <FrenchieApplicationEditorMainMenuLayer.hpp>

#include <FrenchieApplication.hpp>
#include <FrenchieApplicationEditorConsoleLayer.hpp>
#include <FrenchieApplicationEditorPreferencesLayer.hpp>
#include <FrenchieApplicationEditorCommandsQueueLayer.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

#include <FrenchieCoreHelpers.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application::Editor;

// IMGUI
#include <imgui.h>
#include <imgui_internal.h>

// MainMenu
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

// MainMenuFrenchieMenuExitCommand
MainMenuFrenchieMenuExitCommand::MainMenuFrenchieMenuExitCommand(){}
MainMenuFrenchieMenuExitCommand::~MainMenuFrenchieMenuExitCommand(){}

void MainMenuFrenchieMenuExitCommand::execute()
{
    Frenchie::Application::Application::instance()->close();
}

std::string MainMenuFrenchieMenuExitCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::MainMenu), "Frenchie::Exit");
}


// MainMenuWindowsMenuConsoleCommand
MainMenuWindowsMenuConsoleCommand::MainMenuWindowsMenuConsoleCommand(){}
MainMenuWindowsMenuConsoleCommand::~MainMenuWindowsMenuConsoleCommand(){}

void MainMenuWindowsMenuConsoleCommand::execute()
{
    Frenchie::Application::Application::instance()->find_or_push<Console>()->show();
}

std::string MainMenuWindowsMenuConsoleCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::MainMenu), "Windows::Console");
}

// MainMenuWindowsMenuFileSystemCommand
MainMenuWindowsMenuFileSystemCommand::MainMenuWindowsMenuFileSystemCommand(){}
MainMenuWindowsMenuFileSystemCommand::~MainMenuWindowsMenuFileSystemCommand(){}

void MainMenuWindowsMenuFileSystemCommand::execute()
{
    Frenchie::Application::Application::instance()->find_or_push<FileSystemExplorer>()->show();
}

std::string MainMenuWindowsMenuFileSystemCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::MainMenu), "Windows::FileSystem");
}

// MainMenuWindowsMenuSettingsCommand
MainMenuWindowsMenuPreferencesCommand::MainMenuWindowsMenuPreferencesCommand(){}
MainMenuWindowsMenuPreferencesCommand::~MainMenuWindowsMenuPreferencesCommand(){}

void MainMenuWindowsMenuPreferencesCommand::execute()
{
    Frenchie::Application::Application::instance()->find_or_push<Preferences>()->show();
}

std::string MainMenuWindowsMenuPreferencesCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::MainMenu), "Windows::Preferences");
}