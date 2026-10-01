#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserInterfaceStyleTest : public Layer
        {
        public:
            FrenchieImmediateUserInterfaceStyleTest();
            virtual ~FrenchieImmediateUserInterfaceStyleTest();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual void finish() override;
            virtual bool allows_multiple_instances() const override;

        private:

            bool                                                   m_ShowColorPciker  {false};
            bool                                                   m_RGBAColorPicker  {false};
            gs_color                                               m_ColorPickerColor {gs_color_rgb(255, 255, 255)};
            std::map<std::string, ApplicationRenderingBackendFont> m_Fonts            {std::map<std::string, ApplicationRenderingBackendFont>()};
            std::shared_ptr<ImmediateUserInterfaceContextLayer>    m_UI               {nullptr};
        };
    }
}