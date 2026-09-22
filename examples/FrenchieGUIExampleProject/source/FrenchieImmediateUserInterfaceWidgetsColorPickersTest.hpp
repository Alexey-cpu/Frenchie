#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserInterfaceWidgetsColorPickersTest : public Layer
        {
        public:
            FrenchieImmediateUserInterfaceWidgetsColorPickersTest();
            virtual ~FrenchieImmediateUserInterfaceWidgetsColorPickersTest();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual bool allows_multiple_instances() const override;

        private:
            std::shared_ptr<ImmediateUserInterfaceContextLayer> m_UI {nullptr};

            // color pickers
            gs_color       m_ColorPickerColor           {gs_color_rgba(255, 0, 0, 255)};
            bool           m_EnableRGB                  {true};
            bool           m_EnableHSV                  {true};
            bool           m_EnableHSL                  {true};
            bool           m_EnableAlpha                {true};
            bool           m_PreviewColor               {true};
            bool           m_RGBA                       {true};
        };
    }
}