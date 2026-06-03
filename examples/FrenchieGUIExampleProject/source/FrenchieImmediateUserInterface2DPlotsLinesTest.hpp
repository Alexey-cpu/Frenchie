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

            Frenchie::Core::Optional<gs_vec4f> m_Range             {Frenchie::Core::Optional<gs_vec4f>()};
            std::vector<std::vector<float>>    m_XAxisValues       {std::vector<std::vector<float>>()};
            std::vector<std::vector<float>>    m_YAxisValues       {std::vector<std::vector<float>>()};
            std::vector<gs_color>              m_LinesColors       {std::vector<gs_color>()};

            float                              m_XAxisMinValue     {0.f};
            float                              m_XAxisMaxValue     {0.f};

            float                              m_YAxisMinValue     {0.f};
            float                              m_YAxisMaxValue     {0.f};

            int                                m_XAxisTicksCount   {10};
            int                                m_YAxisTicksCount   {10};

            int                                m_PlotSettings      {0    };

            bool                               m_PlotMarkersFilled {true };

            int                                m_AxisSettings      {0    };
            bool                               m_AxisResetScale    {false};
            bool                               m_AxisResetOffset   {false};
            bool                               m_AxisZoomable      {true };
            bool                               m_AxisScrollable    {true };

            int                                m_PlotWidgetSettings   {ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None};

            bool                               m_PlotWidgetDrawLegend {true};
            bool                               m_PlotWidgetFitXAxis   {false};
            bool                               m_PlotWidgetFitYAxis   {false};
        };
    }
}