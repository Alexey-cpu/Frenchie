#pragma once

#include <ImmediateUserInterfaceLayer2.hpp>

namespace Frenchie
{
    namespace Application
    {
        class ImmediateUserInterfaceTestLayer : public Layer
        {
        public:
            ImmediateUserInterfaceTestLayer();
            virtual ~ImmediateUserInterfaceTestLayer();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual void finish() override;

            void windows_test();
            void widgets_test();
            void renderer_test();
            void scrollarea_test();

            void platform_backend_test();

        protected:
            std::shared_ptr<ImmediateUserInterfaceContextLayer> m_ImmediateUserInterface{nullptr};
        };
    }
}