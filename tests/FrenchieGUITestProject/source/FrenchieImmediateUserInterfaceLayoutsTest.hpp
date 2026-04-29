#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserInterfaceLayoutsTest : public Layer
        {
        public:
            FrenchieImmediateUserInterfaceLayoutsTest();
            virtual ~FrenchieImmediateUserInterfaceLayoutsTest();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual bool allows_multiple_instances() const override;

        private:

            // info
            float                                               m_TopMargin                         {0.f};
            float                                               m_LeftMargin                        {0.f};
            float                                               m_RightMargin                       {0.f};
            float                                               m_BottomMargin                      {0.f};
            float                                               m_TopPadding                        {0.f};
            float                                               m_LeftPadding                       {0.f};
            float                                               m_RightPadding                      {0.f};
            float                                               m_BottomPadding                     {0.f};
            int                                                 m_HorizontalAlignmentSettings       {ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter};
            int                                                 m_VerticalAlignmentSettings         {ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter};
            std::string                                         m_HorizontalAligmentComboboxPreview {"None"};
            std::string                                         m_VerticalAligmentComboboxPreview   {"None"};
            std::shared_ptr<ImmediateUserInterfaceContextLayer> m_UI                                {nullptr};
        };
    }
}