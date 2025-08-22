#include <FrenchieApplicationEditorMainMenuLayer.hpp>

#include <FrenchieApplication.hpp>
#include <FrenchieApplicationEditorConsoleLayer.hpp>
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
            class MainMenuPathsParser
            {
            public:
                MainMenuPathsParser() : 
                    m_AbsolutePath(STRINGIFY(Frenchie::Application::Editor::MainMenu)), 
                    m_AbsoluteTokenizedPath(Frenchie::Core::Helpers::String::split(m_AbsolutePath, "::")){}

                ~MainMenuPathsParser(){}
            
                void parse(const std::vector<std::string>& _Path)
                {
                    if(!is_main_menu(_Path)) 
                        return;

                    std::string result;

                    for (size_t i = m_AbsoluteTokenizedPath.size(); i < _Path.size(); i++) 
                        result = (i < _Path.size() - 1) ? result.append(_Path[i]).append("::") : result.append(_Path[i]);

                    if(!result.empty()) 
                        m_RelativePaths.insert(result);
                }

                bool is_main_menu(const std::vector<std::string>& _Path)
                {
                    return _Path.size() >= m_AbsoluteTokenizedPath.size() && 
                        _Path[m_AbsoluteTokenizedPath.size()-1] == m_AbsoluteTokenizedPath.back();
                }

                std::string              m_AbsolutePath          = std::string();
                std::set<std::string>    m_RelativePaths         = std::set<std::string>();
                std::vector<std::string> m_AbsoluteTokenizedPath = std::vector<std::string>();
            };

            class MainMenuPathsDrawer
            {
            public:

                static void draw(std::vector<std::string>& _Actions, std::string& _Path, int& _Index)
                {
                    if(_Index == _Actions.size() - 1) 
                    {
                        if(ImGui::MenuItem(_Actions.back().c_str()))
                        {
                            Frenchie::Application::Application::instance()->find_or_push<CommandsQueue>()->push(_Path);
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
        }
    }
}

// Menu
MainMenu::MainMenu() : Layer(STRINGIFY(MainMenu)){}

MainMenu::~MainMenu(){}

bool MainMenu::awake()
{
    if(!Layer::awake()) 
        return false;

    // parse main menu paths
    MainMenuPathsParser parser;
    for(auto&& creator : Factory::registry()) 
        parser.parse(Frenchie::Core::Helpers::String::split(creator.first, "::"));

    // fill menu infos
    for(auto&& path : parser.m_RelativePaths)
    {
        auto menu = Frenchie::Core::Helpers::String::split(path, "::");

        if(menu.empty()) 
            continue;

        std::vector<std::string> actions;
        for(size_t i = 1; i < menu.size(); i++) 
            actions.push_back(menu[i]);

        m_Menus[menu[0]].Name = menu[0];
        m_Menus[menu[0]].Paths.push_back(fmt::format("{}::{}", parser.m_AbsolutePath, path));
        m_Menus[menu[0]].Actions.push_back((actions.empty() ? std::vector<std::string>({menu[0]}) : actions));
    }

    return true;
}

void MainMenu::frame_update()
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
                    MainMenuPathsDrawer::draw(menu.second.Actions[i], menu.second.Paths[i], index);
                }             

                ImGui::EndMenu();
            }
        }

        ImGui::EndMainMenuBar();
    }
}