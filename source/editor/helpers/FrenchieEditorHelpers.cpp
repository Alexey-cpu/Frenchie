#include <FrenchieApplication.hpp>

// Core
#include <FrenchieCoreStringUtilities.hpp>

// Editor
#include <FrenchieEditorHelpers.hpp>
#include <FrenchieApplicationConfigurationLoaderLanguage.hpp>

using namespace Frenchie::Editor;

using namespace Frenchie::Application::Configuration;

// IMGUI
#include <imgui.h>
#include <imgui_internal.h>

namespace Frenchie
{
    namespace Editor
    {
        namespace Helpers
        {
            inline void draw_menu_recursive(const std::vector<std::string> _Actions, const std::string& _Path, size_t& _Index, const std::shared_ptr<Frenchie::Application::CommandPayload>& _Payload)
            {
                if(_Index == _Actions.size() - 1) 
                {
                    if(ImGui::MenuItem(Translator::translate(_Actions.back()).c_str()))
                        Frenchie::Application::CommandsQueue::instance()->push(_Path, _Payload);

                    return;
                }

                if(ImGui::BeginMenu(Translator::translate(_Actions[_Index]).c_str()))
                {
                    draw_menu_recursive(_Actions, _Path, ++_Index, _Payload);

                    ImGui::EndMenu();
                }
            }
        }
    }
}

void Frenchie::Editor::Helpers::draw_menu(
    const std::string& _MenuPath,
    const std::shared_ptr<Frenchie::Application::CommandPayload>& _Payload)
{
    // parse
    struct Data
    {
        std::vector<std::string> tokens;
        std::string              command;
    };

    std::vector<Data> menus;
    size_t            actionsStartToken = Frenchie::Core::String::split_utf8_string(_MenuPath, "::").size();

    for(auto&& creator : Frenchie::Core::Factory::registry()) 
    {
        if(!Frenchie::Core::String::utf8_string_contains_substring(creator.first, _MenuPath)) 
            continue;

        Data data;
        data.command = creator.first;

        std::vector<std::string> creatorTokens = 
            Frenchie::Core::String::split_utf8_string(creator.first, "::");
        
        for(size_t i = actionsStartToken; i < creatorTokens.size(); i++)
            data.tokens.push_back(creatorTokens[i]);

        menus.push_back(data);
    }

    // draw
    for(auto&& menu : menus)
    {
        size_t index = 0;
        draw_menu_recursive(menu.tokens, menu.command, index, _Payload);
    }
}