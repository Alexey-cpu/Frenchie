#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserIntefaceDragAndDropTest : public Frenchie::Application::Layer
        {
        public:
            FrenchieImmediateUserIntefaceDragAndDropTest();
            virtual ~FrenchieImmediateUserIntefaceDragAndDropTest();

            virtual bool awake() override;
            virtual void frame_update() override;

        protected:
            std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceContextLayer> m_UI         {nullptr};
            gs_color                                                                   m_CanvasColor{gs_color_rgb(255, 255, 255)};
        };
    }
}