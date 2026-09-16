#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserInterfaceEarClippingTest : public Layer
        {
        public:
            FrenchieImmediateUserInterfaceEarClippingTest();
            virtual ~FrenchieImmediateUserInterfaceEarClippingTest();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual bool allows_multiple_instances() const override;

        private:
            std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceContextLayer> m_UI    {nullptr};
            std::shared_ptr<Frenchie::Application::RenderingQueue2D>                   m_Scene {nullptr};
            std::vector<gs_vec2f>                                                      m_Points{std::vector<gs_vec2f>()};
            std::vector<gs_color>                                                      m_Colors{std::vector<gs_color>()};
            int                                                                        m_Moving{-1};

            void render_points(const std::shared_ptr<Frenchie::Application::RenderingQueue2D>& _Renderer);
        };
    }
}