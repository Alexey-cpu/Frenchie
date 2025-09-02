#include <FrenchieApplicationEditorMainMenuFrenchieMenu.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application::Editor;

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