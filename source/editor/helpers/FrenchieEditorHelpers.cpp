#include <FrenchieApplication.hpp>

// Core
#include <FrenchieCoreStringUtilities.hpp>

// Editor
#include <FrenchieEditorHelpers.hpp>
#include <FrenchieApplicationConfigurationLoaderTranslatorLayer.hpp>

using namespace Frenchie::Application;
using namespace Frenchie::Application::Configuration;

using namespace Frenchie::Editor;

// IMGUI
#include <imgui.h>
#include <imgui_internal.h>

namespace Frenchie
{
    namespace Editor
    {
        namespace Helpers
        {
            inline void draw_menu_recursive(
                const std::vector<std::string> _Actions,
                const std::string&             _Path,
                size_t&                        _Index,
                const CommandPayloads&         _Payloads)
            {
                if(_Index == _Actions.size() - 1) 
                {
                    if(ImGui::MenuItem(translator()->translate(_Actions.back()).c_str()))
                        Frenchie::Application::commands()->push(_Path, _Payloads);

                    return;
                }

                if(ImGui::BeginMenu(translator()->translate(_Actions[_Index]).c_str()))
                {
                    draw_menu_recursive(_Actions, _Path, ++_Index, _Payloads);

                    ImGui::EndMenu();
                }
            }

            int ImTextCharToUtf8_inline(char* buf, int buf_size, unsigned int c)
            {
                if (c < 0x80)
                {
                    buf[0] = (char)c;
                    return 1;
                }
                if (c < 0x800)
                {
                    if (buf_size < 2) return 0;
                    buf[0] = (char)(0xc0 + (c >> 6));
                    buf[1] = (char)(0x80 + (c & 0x3f));
                    return 2;
                }
                if (c < 0x10000)
                {
                    if (buf_size < 3) return 0;
                    buf[0] = (char)(0xe0 + (c >> 12));
                    buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
                    buf[2] = (char)(0x80 + ((c ) & 0x3f));
                    return 3;
                }
                if (c <= 0x10FFFF)
                {
                    if (buf_size < 4) return 0;
                    buf[0] = (char)(0xf0 + (c >> 18));
                    buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
                    buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
                    buf[3] = (char)(0x80 + ((c ) & 0x3f));
                    return 4;
                }
                // Invalid code point, the max unicode is 0x10FFFF
                return 0;
            }
        }
    }
}

void Frenchie::Editor::Helpers::imgui_draw_menu(const std::string& _MenuPath, const CommandPayloads& _Payloads)
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
        draw_menu_recursive(menu.tokens, menu.command, index, _Payloads);
    }
}

std::string Frenchie::Editor::Helpers::imgui_convert_text_char_to_utf8(const unsigned int& _Unicode)
{
    char utf8[5];
    int count   = ImTextCharToUtf8_inline(utf8, 5, _Unicode);
    utf8[count] = 0;

    return std::string(utf8, count);
}

ImU32 Frenchie::Editor::Helpers::imgui_calculate_color(const ImVec4& _Vector)
{
    return IM_COL32(_Vector.x * 255.f, _Vector.y * 255.f, _Vector.z * 255.f, _Vector.w * 255.f);
}

ImVec2 Frenchie::Editor::Helpers::imgui_calculate_text_size(const std::string& _Text)
{
    return ImGui::GetCurrentContext()->Font->CalcTextSizeA(
        ImGui::GetFontSize(), FLT_MAX, 0.f, _Text.c_str(), nullptr, nullptr);
}