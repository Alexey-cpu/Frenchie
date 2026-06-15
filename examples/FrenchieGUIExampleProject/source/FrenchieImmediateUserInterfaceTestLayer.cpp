#include <FrenchieImmediateUserInterfaceTestLayer.hpp>

// Application
#include <FrenchieImmediateUserInterface2DPlotsVectorDiagramsTest.hpp>
#include <FrenchieImmediateUserInterface2DPlotsPieChartsTest.hpp>
#include <FrenchieImmediateUserInterface2DPlotsLinesTest.hpp>
#include <FrenchieImmediateUserInterfaceLayoutsTest.hpp>
#include <FrenchieImmediateUserInterfaceWidgetsTest.hpp>
#include <FrenchieImmediateUserInterfaceStyleTest.hpp>

using namespace Frenchie::Application;

FrenchieImmediateUserInterfaceTestLayer::FrenchieImmediateUserInterfaceTestLayer() : Layer(STRINGIFY(FrenchieImmediateUserInterfaceTestLayer)){}
FrenchieImmediateUserInterfaceTestLayer::~FrenchieImmediateUserInterfaceTestLayer(){}

bool FrenchieImmediateUserInterfaceTestLayer::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::Application::push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

    Frenchie::Application::ApplicationPlatformBackend::set_window_name(get_name());

    return m_UI != nullptr;
}

void FrenchieImmediateUserInterfaceTestLayer::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("Interface test window", "InterfaceTestWindow"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults))
    {
        if(m_UI->begin_menubar(m_UI->next_id("Menubar")))
        {
            // menubar
            if(m_UI->begin_menu(m_UI->next_id("Windows", "Windows")))
            {
                if(m_UI->menu_action(m_UI->next_id("Layout test window", "LayoutsTestWindow")))
                    Frenchie::Application::Application::push_layer<FrenchieImmediateUserInterfaceLayoutsTest>();

                if(m_UI->menu_action(m_UI->next_id("Style test window", "StyleTestWindow")))
                    Frenchie::Application::Application::push_layer<FrenchieImmediateUserInterfaceStyleTest>();

                if(m_UI->menu_action(m_UI->next_id("Widgets test window", "WidgetsTestWindow")))
                    Frenchie::Application::Application::push_layer<FrenchieImmediateUserInterfaceWidgetsTest>();

                if(m_UI->begin_menu(m_UI->next_id("2D plots tests", "2DPlotsTests")))
                {
                    if(m_UI->menu_action(m_UI->next_id("2D line plots", "2DLinePlots")))
                        Frenchie::Application::Application::push_layer<FrenchieImmediateUserInterface2DLinePlotsText>();

                    if(m_UI->menu_action(m_UI->next_id("Pie charts", "PieCharts")))
                        Frenchie::Application::Application::push_layer<FrenchieImmediateUserInterfacePieChartsTest>();

                    if(m_UI->menu_action(m_UI->next_id("Vector diagrams", "VectorDiagrams")))
                        Frenchie::Application::Application::push_layer<FrenchieImmediateUserInterface2DPlotsVectorDiagramsTest>();

                    m_UI->end_menu();
                }

                m_UI->end_menu();
            }

            m_UI->end_menubar();
        }

        // overlay
        if(m_UI->begin_scrollarea(
            m_UI->next_id("Overlay"),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically |
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally))
        {
            char longestLabel[] = "Triangles\t";

            float labelWidth = m_UI->m_Renderer->calculate_bounding_box(
                &longestLabel[0],
                &longestLabel[sizeof(longestLabel) / sizeof(char)],
                m_UI->get_text_line_height(),
                m_UI->m_Style.get_current_font()).width();

            // FPS
            m_UI->next_size(gs_vec2f(labelWidth, m_UI->get_text_line_height()));
            m_UI->label(m_UI->next_id("FPSLabel"), "FPS");
            m_UI->same_line();
            m_UI->indent(32.f);
            m_UI->label(m_UI->next_id("FPSValue"), Frenchie::Core::String::to_string(m_UI->m_Renderer->get_rendering_queue_metrics().FrameRate));

            // CMD
            m_UI->next_size(gs_vec2f(labelWidth, m_UI->get_text_line_height()));
            m_UI->label(m_UI->next_id("CMDLabel"), "CMD");
            m_UI->same_line();
            m_UI->indent(32.f);
            m_UI->label(m_UI->next_id("CMDValue"), Frenchie::Core::String::to_string(m_UI->m_Renderer->get_rendering_queue_metrics().RenderingCommandsCount));

            // Triangles
            m_UI->next_size(gs_vec2f(labelWidth, m_UI->get_text_line_height()));
            m_UI->label(m_UI->next_id("TrianglesLabel"), "Triangles");
            m_UI->same_line();
            m_UI->indent(32.f);
            m_UI->label(m_UI->next_id("TrianglesValue"), Frenchie::Core::String::to_string(m_UI->m_Renderer->get_rendering_queue_metrics().RenderedTrianglesCount));

            m_UI->end_scrollarea();
        }

        m_UI->end_window();
    }
}

bool FrenchieImmediateUserInterfaceTestLayer::allows_multiple_instances() const
{
    return false;
}