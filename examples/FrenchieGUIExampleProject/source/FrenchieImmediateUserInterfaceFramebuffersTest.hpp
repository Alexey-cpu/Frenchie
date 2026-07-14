#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserInterfaceFramebuffersTest : public Layer
        {
        public:
            FrenchieImmediateUserInterfaceFramebuffersTest();
            virtual ~FrenchieImmediateUserInterfaceFramebuffersTest();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual bool allows_multiple_instances() const override;

        private:
            std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceContextLayer> m_UI        {nullptr};
            std::shared_ptr<Frenchie::Application::RenderingQueue2D>                   m_Renderer1 {nullptr};
            std::shared_ptr<Frenchie::Application::RenderingQueue2D>                   m_Renderer2 {nullptr};
            std::optional<gs_vec2f>                                                    m_Direction {std::optional<gs_vec2f>()};
            gs_2d_ellipsef                                                             m_Ball      {gs_2d_ellipsef(gs_vec2f(0.f, 0.f), 64.f)};

        };
    }
}