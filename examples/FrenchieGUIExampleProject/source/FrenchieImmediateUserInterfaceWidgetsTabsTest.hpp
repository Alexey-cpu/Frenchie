#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserInterfaceWidgetsTabsTest : public Frenchie::Application::Layer
        {
        public:
            FrenchieImmediateUserInterfaceWidgetsTabsTest();
            virtual ~FrenchieImmediateUserInterfaceWidgetsTabsTest();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual bool allows_multiple_instances() const override;

        protected:
            std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceContextLayer> m_UI {nullptr};

            std::string m_SomeText;
            bool        m_SomeTextOpened{true};
        };
    }
}