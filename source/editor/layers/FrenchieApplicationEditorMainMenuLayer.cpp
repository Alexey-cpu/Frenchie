#include <FrenchieApplicationEditorMainMenuLayer.hpp>

#include <FrenchieCoreHelpers.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;
using namespace Frenchie::Application::Editor::MainMenu;

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
            namespace MainMenu
            {
                class MenuHelpers
                {
                public:

                    static void draw(std::vector<std::string>& _Actions, std::string& _Path, int& _Index)
                    {
                        if(_Index == _Actions.size() - 1) 
                        {
                            if(ImGui::MenuItem(_Actions[_Index].c_str()))
                            {
                                // action here
                            }

                            return;
                        }

                        if(ImGui::BeginMenu(_Actions[_Index].c_str()))
                        {
                            draw(_Actions, _Path, ++_Index);

                            ImGui::EndMenu();
                        }
                    }
                };
            }
        }
    }
}

// Menu
Menu::Menu() : Layer(STRINGIFY(Menu)){}

Menu::~Menu(){}

bool Menu::awake()
{
    if(!Layer::awake()) 
        return false;

    // request menu hierarchy
    std::set<std::string> paths = 
    {
        "Parent_1::child_1::child_2::child_3::action",
        "Parent_1::child_2::child_2::child_3::action",
        "Parent_1::child_3::child_2::child_3::action"
    }; // paths

    for(auto&& path : paths)
    {
        auto menu = Frenchie::Core::Helpers::String::split(path, "::");

        if(menu.empty()) 
            continue;

        std::vector<std::string> actions;
        for(size_t i = 1; i < menu.size(); i++) 
            actions.push_back(menu[i]);

        m_Menus[menu[0]].Name = menu[0];
        m_Menus[menu[0]].Paths.push_back(path);
        m_Menus[menu[0]].Actions.push_back((actions.empty() ? std::vector<std::string>({menu[0]}) : actions));
    }

    return true;
}

void Menu::frame_update()
{
    if(ImGui::BeginMainMenuBar())
    {
        for(auto&& menu : m_Menus)
        {
            if(ImGui::BeginMenu(menu.first.c_str()))
            {
                for (size_t i = 0; i < menu.second.Actions.size(); i++)
                {
                    int index = 0;
                    MenuHelpers::draw(menu.second.Actions[i], menu.second.Paths[i], index);
                }             

                ImGui::EndMenu();
            }
        }

        ImGui::EndMainMenuBar();
    }
}