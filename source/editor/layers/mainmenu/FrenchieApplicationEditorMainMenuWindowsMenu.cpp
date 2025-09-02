#include <FrenchieApplicationEditorMainMenuWindowsMenu.hpp>

#include <FrenchieApplicationEditorConsoleLayer.hpp>
#include <FrenchieApplicationEditorPreferencesLayer.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application::Editor;

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