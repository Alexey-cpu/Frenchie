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
            void scrollarea_test();
            void vertical_stack_test();
            void horizontal_stack_test();

        protected:
            std::shared_ptr<ImmediateUserInterfaceContextLayer> m_ImmediateUserInterface{nullptr};
        };
    }
}