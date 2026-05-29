#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserInterfacePieChartsTest : public Layer
        {
        public:
            FrenchieImmediateUserInterfacePieChartsTest();
            virtual ~FrenchieImmediateUserInterfacePieChartsTest();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual bool allows_multiple_instances() const override;

        protected:

            std::shared_ptr<ImmediateUserInterfaceContextLayer> m_UI        {nullptr};
            std::vector<float>                                  m_PieValues {std::vector<float>()      };
            std::vector<gs_color>                               m_PieColors {std::vector<gs_color>()   };
            std::vector<std::string>                            m_PieNames  {std::vector<std::string>()};
        };
    }
}