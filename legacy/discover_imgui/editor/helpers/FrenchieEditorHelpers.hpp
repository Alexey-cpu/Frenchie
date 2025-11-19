#pragma once

// Application
#include <FrenchieApplication.hpp>

// STL
#include <string>
#include <vector>
#include <map>
#include <set>

// IMGUI
#include <imgui.h>
#include <imgui_internal.h>

namespace Frenchie
{
    namespace Editor
    {
        namespace Helpers
        {
            using namespace Frenchie::Application;

            void imgui_draw_menu(const std::string& _MenuPath, const CommandPayloads& _Payloads = CommandPayloads());

            std::string imgui_convert_text_char_to_utf8(const unsigned int&);

            ImU32  imgui_calculate_color(const ImVec4&);

            ImVec2 imgui_calculate_text_size(const std::string&);
        }
    }
}