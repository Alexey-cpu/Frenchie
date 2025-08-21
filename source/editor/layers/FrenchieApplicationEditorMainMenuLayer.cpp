#include <FrenchieApplicationEditorMainMenuLayer.hpp>

#include <FrenchieCoreHelpers.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

// IMGUI
#include <imgui.h>
#include <imgui_internal.h>

MainMenu::MainMenu() : Layer(STRINGIFY(MainMenu)){}

MainMenu::~MainMenu(){}

bool MainMenu::awake()
{
    if(!Layer::awake()) 
        return false;

    return true;
}

void MainMenu::frame_update()
{
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New")){}

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}