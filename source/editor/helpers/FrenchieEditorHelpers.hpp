#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

// STL
#include <string>
#include <vector>
#include <map>
#include <set>

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
        }
    }
}