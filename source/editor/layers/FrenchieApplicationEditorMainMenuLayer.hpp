#pragma once

#include <FrenchieApplicationLayer.hpp>

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

                virtual bool awake() override;
                virtual void frame_update() override;

                // virtual bool on_awake();
                // virtual void on_frame_start();
                // virtual void on_frame_update();
                // virtual void on_frame_finish();
                // virtual void on_finish();
            };
        }
    }
}