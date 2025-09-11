#include <FrenchieApplication.hpp>

// Application
#include <FrenchieApplicationCommandsLayer.hpp>

// Editor
#include <FrenchieEditorHelpers.hpp>
#include <FrenchieEditorConfigurationTranslatorLayer.hpp>

using namespace Frenchie::Editor;
using namespace Frenchie::Editor::Configuration;

// IMGUI
#include <imgui.h>
#include <imgui_internal.h>

namespace Frenchie
{
    namespace Editor
    {
        namespace Helpers
        {
            void draw_menu_recursive(const std::vector<std::string> _Actions, const std::string& _Path, size_t& _Index, void* _Sender)
            {
                if(_Index == _Actions.size() - 1) 
                {
                    if(ImGui::MenuItem(Translator::translate(_Actions.back()).c_str()))
                        Frenchie::Application::CommandsQueue::instance()->push(_Path, _Sender);

                    return;
                }

                if(ImGui::BeginMenu(Translator::translate(_Actions[_Index]).c_str()))
                {
                    draw_menu_recursive(_Actions, _Path, ++_Index, _Sender);

                    ImGui::EndMenu();
                }
            }
        }
    }
}

void Frenchie::Editor::Helpers::draw_menu(const std::string& _MenuPath, void* _Sender)
{
    // parse
    struct Data
    {
        std::vector<std::string> tokens;
        std::string              command;
    };

    std::vector<Data> menus;
    size_t            actionsStartToken = Frenchie::Core::String::split(_MenuPath, "::").size();

    for(auto&& creator : Frenchie::Core::Factory::registry()) 
    {
        if(!Frenchie::Core::String::contains_substring(creator.first, _MenuPath)) 
            continue;

        Data data;
        data.command = creator.first;

        std::vector<std::string> creatorTokens = 
            Frenchie::Core::String::split(creator.first, "::");
        
        for(size_t i = actionsStartToken; i < creatorTokens.size(); i++)
            data.tokens.push_back(creatorTokens[i]);

        menus.push_back(data);
    }

    // draw
    for(auto&& menu : menus)
    {
        size_t index = 0;
        draw_menu_recursive(menu.tokens, menu.command, index, _Sender);
    }
}