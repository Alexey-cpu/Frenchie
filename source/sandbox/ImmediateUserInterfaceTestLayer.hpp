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
        protected:
            std::shared_ptr<ImmedidateUserInterfaceContextLayer> m_ImmediateUserInterface{nullptr};
        };
    }
}