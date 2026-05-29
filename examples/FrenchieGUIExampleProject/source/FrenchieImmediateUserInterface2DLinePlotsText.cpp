#include <FrenchieImmediateUserInterface2DLinePlotsText.hpp>

using namespace Frenchie::Application;

FrenchieImmediateUserInterface2DLinePlotsText::FrenchieImmediateUserInterface2DLinePlotsText() : Layer(STRINGIFY(FrenchieImmediateUserInterface2DLinePlotsText)){}
FrenchieImmediateUserInterface2DLinePlotsText::~FrenchieImmediateUserInterface2DLinePlotsText(){}

bool FrenchieImmediateUserInterface2DLinePlotsText::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::application()->push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

    // generate data
    float fn = 50.f;
    float Ns = 80.f;
    float fs = fn * Ns;
    float Ts = 1.f / fs;
    float T  = 20.f / 1000.f;
    int   nn = 0;

    for (int i = 0; i < 5; i++)
    {
        std::vector<float> X;
        std::vector<float> Y;
        float              A = gs_pseudo_random<float>(0.1f, 2.f);

        for (int n = 0; n <= (int)(T * fs) * 1; n++)
        {
            if(nn >= gs_pseudo_random<float>(0, 4))
            {
                nn = 0;
                continue;
            }

            Y.push_back(A * sin(PI2 * fn * Ts * (float)n + gs_to_radians(i * 30.f)));
            X.push_back(n);
            nn++;
        }

        m_XAxisValues.push_back(X);
        m_YAxisValues.push_back(Y);
        m_LinesColors.push_back(gs_color_rgba(gs_pseudo_random<int>(0, 255), gs_pseudo_random<int>(0, 255), gs_pseudo_random<int>(0, 255), 255));
    }

    // plot settings
    m_PlotSettings  = 
          Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsLines
        | Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderLabelsOnHover;

    // axis settings
    m_AxisSettings  = Frenchie::Application::ImmediateUserInterfacePlotLineAxisSettings_::ImmediateUserInterfacePlotLineAxisSettings_Defaults;
    m_XAxisMinValue = +0.f;
    m_XAxisMaxValue = +30.f;
    m_YAxisMinValue = -2.f;
    m_YAxisMaxValue = +2.f;

    return m_UI != nullptr;
}

void FrenchieImmediateUserInterface2DLinePlotsText::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("2D line plots", "2DLinePlots"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        if(m_UI->begin_horizontal_stack(m_UI->next_id("Root")))
        {
            // editor
            m_UI->next_content_margin(m_UI->get_content_default_margin());
            m_UI->next_content_padding(m_UI->get_content_default_margin());
            
            if(m_UI->begin_scrollarea(m_UI->next_id("Editor")))
            {
                // restore
                {
                    m_AxisResetScale  = m_UI->push_button(m_UI->next_id("Reset axis scale", "Reset axis scale"));
                    m_AxisResetOffset = m_UI->push_button(m_UI->next_id("Reset axis offset", "Reset axis offset"));
                }

                // render mode
                {
                    std::string renderModePreview = "None";

                    if(m_PlotSettings & Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsLines)
                        renderModePreview = "Lines";
                    else if(m_PlotSettings & Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsStems)
                        renderModePreview = "Stems";
                    else if(m_PlotSettings & Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsPoints)
                        renderModePreview = "Points";
                    else if(m_PlotSettings & Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsRectangles)
                        renderModePreview = "Rectangles";
                    else if(m_PlotSettings & Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsConvexAreas)
                        renderModePreview = "Convex areas";

                    if(m_UI->begin_combobox(m_UI->next_id("RenderMode"), renderModePreview))
                    {
                        if(m_UI->combobox_item(m_UI->next_id("Lines", "Lines")))
                        {
                            reset_render_mode();
                            m_PlotSettings |= Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsLines;
                        }

                        if(m_UI->combobox_item(m_UI->next_id("Stems", "Stems")))
                        {
                            reset_render_mode();
                            m_PlotSettings |= Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsStems;
                        }

                        if(m_UI->combobox_item(m_UI->next_id("Points", "Points")))
                        {
                            reset_render_mode();
                            m_PlotSettings |= Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsPoints;   
                        }

                        if(m_UI->combobox_item(m_UI->next_id("Rectangles", "Rectangles")))
                        {
                            reset_render_mode();
                            m_PlotSettings |= Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsRectangles;   
                        }

                        if(m_UI->combobox_item(m_UI->next_id("Convex areas", "ConvexAreas")))
                        {
                            reset_render_mode();
                            m_PlotSettings |= Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsConvexAreas;   
                        }

                        m_UI->end_combobox();
                    }

                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("RenderModeLabel"), "Render mode");
                }

                // markers
                {
                    std::string markersPreview = "None";

                    if(m_PlotSettings & Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersPoints)
                        markersPreview = "Points";
                    else if(m_PlotSettings & Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersTriangles)
                        markersPreview = "Triangles";
                    else if(m_PlotSettings & Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersRectangles)
                        markersPreview = "Rectangles";
                    else
                        markersPreview = "None";

                    if(m_UI->begin_combobox(m_UI->next_id("Markers"), markersPreview))
                    {
                        if(m_UI->combobox_item(m_UI->next_id("None", "None")))
                            reset_markers();

                        if(m_UI->combobox_item(m_UI->next_id("Points", "Points")))
                        {
                            reset_markers();
                            m_PlotSettings |= Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersPoints;
                        }

                        if(m_UI->combobox_item(m_UI->next_id("Triangles", "Triangles")))
                        {
                            reset_markers();
                            m_PlotSettings |= Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersTriangles;
                        }

                        if(m_UI->combobox_item(m_UI->next_id("Rectangles", "Rectangles")))
                        {
                            reset_markers();
                            m_PlotSettings |= Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersRectangles;
                        }

                        m_UI->end_combobox();
                    }

                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("MarkesLabel"), "Markers");
                }

                // markers type
                {
                    if(m_UI->check_button(m_UI->next_id("MarkesType"), m_PlotMarkersFilled))
                        m_PlotSettings &= ~Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersOpened;
                    else
                        m_PlotSettings |=  Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersOpened;

                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("MarkesTypeLabel"), "Markes filled");
                }

                // XY axis ticks count
                {
                    m_UI->input_scalar_slider(m_UI->next_id("XAxisTicksCountSlider"), m_XAxisTicksCount, 4, 20);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("XAxisTicksCountLabel"), "X-axis ticks count");

                    m_UI->input_scalar_slider(m_UI->next_id("YAxisTicksCountSlider"), m_YAxisTicksCount, 4, 20);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("YAxisTicksCountLabel"), "Y-axis ticks count");
                }

                // XY axis ranges
                {
                    m_UI->input_scalar_slider<float>(m_UI->next_id("XAxisMinValiue"), m_XAxisMinValue, 0.f, 100.f);
                    m_UI->same_line();
                    m_UI->input_scalar_slider<float>(m_UI->next_id("XAxisMaxValiue"), m_XAxisMaxValue, 0.f, 100.f);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("XAxisRangeLabel"), Frenchie::Core::String::format("X: [%.2f, %.2f]", m_XAxisMinValue, m_XAxisMaxValue));

                    m_UI->input_scalar_slider<float>(m_UI->next_id("YAxisMinValiue"), m_YAxisMinValue, 0.f, -4.f);
                    m_UI->same_line();
                    m_UI->input_scalar_slider<float>(m_UI->next_id("YAxisMaxValiue"), m_YAxisMaxValue, 0.f, +4.f);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("YAxisRangeLabel"), Frenchie::Core::String::format("Y: [%.2f, %.2f]", m_YAxisMinValue, m_YAxisMaxValue));
                }

                m_UI->end_scrollarea();
            }

            // plots
            if(m_UI->begin_plot(m_UI->next_id("Plots")))
            {
                for (int i = 0; i < (int)m_XAxisValues.size(); i++)
                {
                    auto xAxisName = Frenchie::Core::String::format("XAxis-%d", i);
                    auto yAxisName = Frenchie::Core::String::format("YAxis-%d", i);

                    if(m_AxisResetScale ) m_UI->next_axis_scale(gs_vec2f(1.f, 1.f));
                    if(m_AxisResetOffset) m_UI->next_axis_offset(gs_vec2f(0.f, 0.f));
                    m_UI->plot_axis_x(m_UI->next_id(xAxisName, xAxisName), m_XAxisMinValue, m_XAxisMaxValue, m_XAxisTicksCount);

                    if(m_UI->begin_what_is_it(m_UI->next_id("XAxisPopup"), m_UI->get_rendered_stack_top()))
                    {
                        m_UI->label(m_UI->next_id("XAxisText"), "Press left CTRL and move a wheel to scale axis");
                        m_UI->end_what_is_it();
                    }

                    if(m_AxisResetScale ) m_UI->next_axis_scale(gs_vec2f(1.f, 1.f));
                    if(m_AxisResetOffset) m_UI->next_axis_offset(gs_vec2f(0.f, 0.f));
                    m_UI->plot_axis_y(m_UI->next_id(yAxisName, yAxisName), m_YAxisMinValue, m_YAxisMaxValue, m_YAxisTicksCount);

                    if(m_UI->begin_what_is_it(m_UI->next_id("YAxisPopup"), m_UI->get_rendered_stack_top()))
                    {
                        m_UI->label(m_UI->next_id("XAxisText"), "Press left CTRL and move a wheel to scale axis");
                        m_UI->end_what_is_it();
                    }

                    auto plotID = Frenchie::Core::String::format("Plot-%d", i);

                    m_Range = m_UI->plot_line(
                        m_UI->next_id(plotID, plotID),
                            &m_XAxisValues[i][0],
                            &m_YAxisValues[i][0],
                            (int)m_XAxisValues[i].size(),
                            m_LinesColors[i],
                            12.f,
                            m_PlotSettings,
                            m_Range);
                }

                m_UI->end_plot();
            }

            m_UI->end_horizontal_stack();
        }

        m_UI->end_window();
    }
}

bool FrenchieImmediateUserInterface2DLinePlotsText::allows_multiple_instances() const
{
    return false;
}

void FrenchieImmediateUserInterface2DLinePlotsText::reset_render_mode()
{
    m_PlotSettings &= ~Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsLines;
    m_PlotSettings &= ~Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsStems;
    m_PlotSettings &= ~Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsPoints;
    m_PlotSettings &= ~Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsRectangles;
    m_PlotSettings &= ~Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsConvexAreas;
}

void FrenchieImmediateUserInterface2DLinePlotsText::reset_markers()
{
    m_PlotSettings &= ~Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersPoints;
    m_PlotSettings &= ~Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersTriangles;
    m_PlotSettings &= ~Frenchie::Application::ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersRectangles;   
}