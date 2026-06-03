#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserInterface2DPlotsVectorDiagramsTest : public Layer
        {
        public:
            FrenchieImmediateUserInterface2DPlotsVectorDiagramsTest();
            virtual ~FrenchieImmediateUserInterface2DPlotsVectorDiagramsTest();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual bool allows_multiple_instances() const override;

        protected:

            std::shared_ptr<ImmediateUserInterfaceContextLayer> m_UI            {nullptr};

            std::vector<gs_vec4f>    m_Vectror;
            std::vector<gs_color>    m_Colors;
            std::vector<std::string> m_Names;

            int                                                 m_PlotWidgetSettings  {ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None};
            bool                                                m_PlotWidgetDrawLegend{true};
        };
    }
}