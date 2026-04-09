#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserInterfaceTestLayer : public Layer
        {
        public:
            FrenchieImmediateUserInterfaceTestLayer();
            virtual ~FrenchieImmediateUserInterfaceTestLayer();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual bool allows_multiple_instances() const override;

        private:
            std::shared_ptr<ImmediateUserInterfaceContextLayer> m_UI {nullptr};
        };
    }
}