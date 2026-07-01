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

            gs_2d_ellipsef m_Ball          {gs_2d_ellipsef(gs_vec2f(0.f, 0.f), 64.f, 64.f)};
            gs_vec2f       m_BallDirection {gs_vec2f(0.f, 0.f)};
            float          m_BallMoveSpeed {4.f};
        };
    }
}