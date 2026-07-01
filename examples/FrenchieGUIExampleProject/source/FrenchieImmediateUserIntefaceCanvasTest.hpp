#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserIntefaceCanvasTest : public Frenchie::Application::Layer
        {
        public:
            FrenchieImmediateUserIntefaceCanvasTest();
            virtual ~FrenchieImmediateUserIntefaceCanvasTest();

            virtual bool awake() override;
            virtual void frame_update() override;

        protected:
            std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceContextLayer> m_UI {nullptr};

            gs_color m_ColorPickerColor     = gs_color_rgb(255, 255, 255);
            gs_color m_ClockFrameColor      = gs_color_rgb(128, 128, 128);
            gs_color m_ClockBackgroundColor = gs_color_rgb(32, 32, 32);
        };
    }
}