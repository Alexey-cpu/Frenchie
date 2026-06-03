#include <FrenchieImmediateUserInterface2DPlotsPieChartsTest.hpp>

using namespace Frenchie::Application;

FrenchieImmediateUserInterfacePieChartsTest::FrenchieImmediateUserInterfacePieChartsTest() : Layer(STRINGIFY(FrenchieImmediateUserInterfacePieChartsTest)){}
FrenchieImmediateUserInterfacePieChartsTest::~FrenchieImmediateUserInterfacePieChartsTest(){}

bool FrenchieImmediateUserInterfacePieChartsTest::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::application()->push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

    for (int i = 0; i < 5; i++)
    {
        m_PieValues.push_back(5.f * (i + 1));
        m_PieColors.push_back(gs_color_rgba(gs_pseudo_random<int>(0, 255), gs_pseudo_random<int>(0, 255), gs_pseudo_random<int>(0, 255), 255));
        m_PieNames.push_back(Frenchie::Core::String::format("Sector-%d", i));
    }

    return m_UI != nullptr;
}

void FrenchieImmediateUserInterfacePieChartsTest::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("Pie charts", "PieCharts"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        if(m_UI->begin_horizontal_stack(m_UI->next_id("Root")))
        {
            if(m_UI->begin_scrollarea(m_UI->next_id("Settings")))
            {
                if(m_UI->check_button(m_UI->next_id("DrawLegendCheckbox"), m_PlotWidgetDrawLegend))
                    m_PlotWidgetSettings |=  ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_PlotDrawLegend;
                else
                    m_PlotWidgetSettings &= ~ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_PlotDrawLegend;

                m_UI->same_line();
                m_UI->label(m_UI->next_id("DrawLegendLabel"), "Draw legend");

                m_UI->end_scrollarea();
            }

            if(m_UI->begin_plot(m_UI->next_id("Plots"), m_PlotWidgetSettings))
            {
                m_UI->plot_pie(&m_PieNames[0], &m_PieValues[0], &m_PieColors[0], (int)m_PieValues.size());
                m_UI->end_plot();
            }

            m_UI->end_horizontal_stack();
        }

        m_UI->end_window();
    }
}

bool FrenchieImmediateUserInterfacePieChartsTest::allows_multiple_instances() const
{
    return false;
}