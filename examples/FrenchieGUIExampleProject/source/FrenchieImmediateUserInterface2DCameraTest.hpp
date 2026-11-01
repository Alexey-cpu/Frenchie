#pragma once

#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserInterface2DCameraTest : public Layer
        {
        public:
            FrenchieImmediateUserInterface2DCameraTest();
            virtual ~FrenchieImmediateUserInterface2DCameraTest();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual void finish() override;
            virtual bool allows_multiple_instances() const override;

        private:
            std::shared_ptr<Frenchie::Application::ImmediateUserInterface> m_UI           {nullptr};
            std::shared_ptr<Frenchie::Application::RenderingQueue2D>       m_Renderer     {nullptr};
            float                                                          m_AmbientStrength {1.f};
        };
    }
}