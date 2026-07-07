#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserInterfaceDynamicStyleTest : public Layer
        {
        public:
            FrenchieImmediateUserInterfaceDynamicStyleTest();
            virtual ~FrenchieImmediateUserInterfaceDynamicStyleTest();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual bool allows_multiple_instances() const override;

        private:

            bool                                                m_ShowColorPciker  {false};
            bool                                                m_RGBAColorPicker  {false};
            int                                                 m_ColorPickerColor {0};
            std::shared_ptr<ImmediateUserInterfaceContextLayer> m_UI               {nullptr};
            ImmediateUserInterfaceStyle                         m_Style;
        };
    }
}