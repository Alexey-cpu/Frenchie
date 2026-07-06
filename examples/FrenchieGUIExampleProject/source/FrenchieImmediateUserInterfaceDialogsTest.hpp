#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserInterfaceDialogsTest : public Frenchie::Application::Layer
        {
        public:
            FrenchieImmediateUserInterfaceDialogsTest();
            virtual ~FrenchieImmediateUserInterfaceDialogsTest();

            virtual bool awake() override;
            virtual void frame_update() override;

        protected:
            std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceContextLayer> m_UI {nullptr};

            bool m_ShowParentDialog{false};
            bool m_ShowChildDialog {false};
        };
    }
}