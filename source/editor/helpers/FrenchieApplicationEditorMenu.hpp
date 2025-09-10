#pragma once

#include <FrenchieApplicationLayer.hpp>

// STL
#include <string>
#include <vector>
#include <map>
#include <set>

namespace Frenchie
{
    namespace Editor
    {
        class Menu
        {
        public:
            Menu();
            ~Menu();
        
            // Layer
            void draw(const std::string& _MenuPath, void* _Sender = nullptr, bool _ForceUpdate = false);

        protected:

            // nested types
            struct MenuData
            {
                std::string                           Name;
                std::vector<std::string>              Paths;
                std::vector<std::vector<std::string>> Actions;
            };

            // info
            std::map<std::string, MenuData> m_Menus;
        };
    }
}