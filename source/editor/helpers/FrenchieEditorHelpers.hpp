#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

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

            void draw_menu(
                const std::string&     _MenuPath,
                const CommandPayloads& _Payload = CommandPayloads());

            std::string convert_imgui_text_char_to_utf8(unsigned int c);

            ImU32 calculate_color(const ImVec4& _Vector);

            ImVec2 calculate_text_size(const std::string& _Text);
        }
    }
}