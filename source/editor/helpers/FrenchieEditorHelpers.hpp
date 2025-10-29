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
            void draw_menu(const std::string& _MenuPath, const std::shared_ptr<Frenchie::Application::CommandPayload>& _Payload = nullptr);
        }
    }
}