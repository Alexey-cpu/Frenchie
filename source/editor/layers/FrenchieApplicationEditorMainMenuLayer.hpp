#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>

// Core

// STL
#include <map>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            namespace MainMenu
            {
                class Menu : public Layer
                {
                public:
                    Menu();
                    virtual ~Menu();

                    virtual bool awake() override;
                    virtual void frame_update() override;

                protected:

                    struct MenuData
                    {
                        std::string                           Name;
                        std::vector<std::string>              Paths;
                        std::vector<std::vector<std::string>> Actions;
                    };

                    std::map<std::string, MenuData> m_Menus;
                };
            }
        }
    }
}