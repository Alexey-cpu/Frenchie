#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserInterface2DLinePlotsText : public Layer
        {
        public:
            FrenchieImmediateUserInterface2DLinePlotsText();
            virtual ~FrenchieImmediateUserInterface2DLinePlotsText();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual bool allows_multiple_instances() const override;

        protected:

            void reset_render_mode();
            void reset_markers();

            std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceContextLayer> m_UI{nullptr};

            std::vector<std::vector<float>>    m_XAxisValues;
            std::vector<std::vector<float>>    m_YAxisValues;
            std::vector<gs_color>              m_PlotsLineColors;
            gs_vec2f                           m_MinAxis;
            gs_vec2f                           m_MaxAxis;
            int                                m_XAxisTicksCount {10};
            int                                m_YAxisTicksCount {10};

            Frenchie::Core::Optional<gs_vec4f> m_Range          {Frenchie::Core::Optional<gs_vec4f>()};
            int                                m_Settings       {0    };
            bool                               m_MarkersFilled  {true };
            bool                               m_ResetPlotScale {false};
            bool                               m_ResetPlotOffset{false};
            bool                               m_Editable       {true };
            bool                               m_Zoomable       {true };
            bool                               m_Draggable      {true };
            bool                               m_RenderPoints   {true };
            bool                               m_RenderLabels   {true };
        };
    }
}