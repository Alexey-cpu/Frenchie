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