#include <FrenchieImmediateUserInterface2DPlotsVectorDiagramsTest.hpp>

using namespace Frenchie::Application;

FrenchieImmediateUserInterface2DPlotsVectorDiagramsTest::FrenchieImmediateUserInterface2DPlotsVectorDiagramsTest() : Layer(STRINGIFY(FrenchieImmediateUserInterface2DPlotsVectorDiagramsTest)){}
FrenchieImmediateUserInterface2DPlotsVectorDiagramsTest::~FrenchieImmediateUserInterface2DPlotsVectorDiagramsTest(){}

bool FrenchieImmediateUserInterface2DPlotsVectorDiagramsTest::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

    for (int i = 0; i < 10; i++)
    {
        float angle = gs_to_radians(30.f * i);

        gs_vec2f vector = gs_vec2f(cos(angle), sin(angle)) * gs_pseudo_random<float>(30.f, 200.f);

        m_Vectror.push_back(gs_vec4f(0.f, 0.f, vector.x, vector.y));
        m_Colors.push_back(gs_color_rgba(gs_pseudo_random<int>(0, 255), gs_pseudo_random<int>(0, 255), gs_pseudo_random<int>(0, 255), 255));
        m_Names.push_back(Frenchie::Core::String::format("Vector-%d", i));
    }

    return m_UI != nullptr;
}

void FrenchieImmediateUserInterface2DPlotsVectorDiagramsTest::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("Vector diagrams", "VectorDiagrams"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        if(m_UI->begin_vertical_stack(m_UI->next_id("Root")))
        {
            m_UI->next_maximum_height(m_UI->get_text_line_height());

            if(m_UI->begin_scrollarea(m_UI->next_id("Settings")))
            {
                m_UI->same_line();
                m_UI->label(m_UI->next_id("DrawLegendLabel"), "Draw legend");
                m_UI->same_line();
                m_UI->check_button(m_UI->next_id("DrawLegendCheckBox"), m_PlotWidgetDrawLegend);

                m_UI->end_scrollarea();
            }

            if(m_UI->begin_plot(m_UI->next_id("Plots"), m_PlotWidgetSettings))
            {
                m_UI->plot_vector(&m_Names[0], &m_Vectror[0], &m_Colors[0], (int)m_Vectror.size());
                m_UI->end_plot();
            }

            if(m_PlotWidgetDrawLegend)
                m_UI->plot_legend(m_UI->next_id("Legend"), m_UI->get_rendered_stack_top());

            m_UI->end_vertical_stack();
        }

        m_UI->end_window();
    }
}

bool FrenchieImmediateUserInterface2DPlotsVectorDiagramsTest::allows_multiple_instances() const
{
    return false;
}