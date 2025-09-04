#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationEditorMenuDrawer.hpp>

// Core

// STL
#include <map>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class MainMenu : public Layer
            {
            public:
                MainMenu();
                virtual ~MainMenu();

                // Frenchie::Application::Layer
                virtual void frame_update() override;

            protected:

                MenuDrawer m_MenuDrawer;
            };
        }
    }
}