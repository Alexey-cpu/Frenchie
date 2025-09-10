#include <FrenchieApplication.hpp>

#include <FrenchieApplicationEditorMenu.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>
#include <FrenchieApplicationEditorConfigurationTranslatorLayer.hpp>

using namespace Frenchie::Editor;
using namespace Frenchie::Editor::Configuration;

// IMGUI
#include <imgui.h>
#include <imgui_internal.h>

namespace Frenchie
{
    namespace Editor
    {
        class MenuPathsParser
        {
        public:
            MenuPathsParser(const std::string& _AbsolutePath) : 
                m_AbsolutePath(_AbsolutePath), 
                m_AbsoluteTokenizedPath(Frenchie::Core::String::split(m_AbsolutePath, "::")){}

            ~MenuPathsParser(){}
        
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

        class MenuPathsDrawer
        {
        public:

            static void draw(std::vector<std::string>& _Actions, std::string& _Path, int& _Index, void* _Sender)
            {
                if(_Index == _Actions.size() - 1) 
                {
                    if(ImGui::MenuItem(Translator::translate(_Actions.back()).c_str()))
                        Frenchie::Application::application()->push_command(_Path, _Sender);

                    return;
                }

                if(ImGui::BeginMenu(Translator::translate(_Actions[_Index]).c_str()))
                {
                    draw(_Actions, _Path, ++_Index, _Sender);

                    ImGui::EndMenu();
                }
            }
        };
    }
}

Menu::Menu(){}

Menu::~Menu(){}

void Menu::draw(const std::string& _MenuPath, void* _Sender, bool _ForceUpdate)
{
    if(_ForceUpdate) 
        m_Menus.clear();

    // parse menu
    if(m_Menus.empty() && !_MenuPath.empty())
    {
        // parse main menu paths
        MenuPathsParser parser(_MenuPath);
        for(auto&& creator : Frenchie::Core::Factory::registry()) 
            parser.parse(Frenchie::Core::String::split(creator.first, "::"));

        // fill menu infos
        for(auto&& path : parser.m_RelativePaths)
        {
            auto menu = Frenchie::Core::String::split(path, "::");

            if(menu.empty()) 
                continue;

            std::vector<std::string> actions;
            for(size_t i = 1; i < menu.size(); i++) 
                actions.push_back(menu[i]);

            m_Menus[menu[0]].Name = menu[0];
            m_Menus[menu[0]].Paths.push_back(fmt::format("{}::{}", parser.m_AbsolutePath, path));
            m_Menus[menu[0]].Actions.push_back((actions.empty() ? std::vector<std::string>({menu[0]}) : actions));
        }
    }

    // draw menu
    int index = 0;
    
    for(auto&& menu : m_Menus)
    {
        if(menu.second.Actions.size() <= 1 && 
            menu.second.Actions.front().size() <= 1 && 
            menu.second.Actions.front().front() == menu.second.Name) // identify if it's menu or not
        {
            for (size_t i = 0; i < menu.second.Actions.size(); i++)
                MenuPathsDrawer::draw(menu.second.Actions[i], menu.second.Paths[i], index, _Sender);
        }
        else
        {
            if(ImGui::BeginMenu(Translator::translate(menu.first).c_str()))
            {
                for (size_t i = 0; i < menu.second.Actions.size(); i++)
                    MenuPathsDrawer::draw(menu.second.Actions[i], menu.second.Paths[i], index, _Sender);             

                ImGui::EndMenu();
            }
        }
    }
}