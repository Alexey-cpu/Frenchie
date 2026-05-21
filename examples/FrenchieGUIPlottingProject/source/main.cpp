#include <FrenchieApplication.hpp>
#include <FrenchieImmediateUserInterfaceLayer.hpp>

#include <iostream>

#include <stdio.h>
#include <stdarg.h>

class SomeSimpleGuiLayer : public Frenchie::Application::Layer
{
public:
    SomeSimpleGuiLayer() : Frenchie::Application::Layer("TestLayer"){}
    virtual ~SomeSimpleGuiLayer(){}

    virtual bool awake() override
    {
        if(m_UI == nullptr)
            m_UI = Frenchie::Application::application()->push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

        // generate data
        float fn = 50.f;
        float Ns = 80.f;
        float fs = fn * Ns;  // Hs
        float Ts = 1.f / fs; // s
        float T  = 20.f / 1000.f;

        int nn = 0;

        for (int i = 0; i < 5; i++)
        {
            std::vector<float> X;
            std::vector<float> Y;
            float              A = gs_pseudo_random<float>(0.1f, 2.f);

            for (int n = 0; n <= (int)(T * fs) * 1000; n++)
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

            XAxisValues.push_back(X);
            YAxisValues.push_back(Y);
            LineColors.push_back(gs_color_rgb(gs_pseudo_random<int>(0, 255), gs_pseudo_random<int>(0, 255), gs_pseudo_random<int>(0, 255)));
        }

        MinAxis = gs_vec2f(0.f, -2.f);
        MaxAxis = gs_vec2f(80, +2.f);

        return m_UI != nullptr;
    }

    virtual void frame_update() override
    {
        if(m_UI->begin_window(m_UI->next_id("SomeSimpleWindow")))
        {
            if(m_UI->begin_horizontal_stack(m_UI->next_id("Plot")))
            {                
                int plotSettings = 0;
                if(Editable    ) plotSettings |= Frenchie::Application::ImmediateUserInterfacePlotXYSettings_::ImmediateUserInterfacePlotXYSettings_Editable;
                if(Zoomable    ) plotSettings |= Frenchie::Application::ImmediateUserInterfacePlotXYSettings_::ImmediateUserInterfacePlotXYSettings_Zoomable;
                if(Draggable   ) plotSettings |= Frenchie::Application::ImmediateUserInterfacePlotXYSettings_::ImmediateUserInterfacePlotXYSettings_Draggable;
                if(RenderPoints) plotSettings |= Frenchie::Application::ImmediateUserInterfacePlotXYSettings_::ImmediateUserInterfacePlotXYSettings_RenderPoints;
                if(RenderLabels) plotSettings |= Frenchie::Application::ImmediateUserInterfacePlotXYSettings_::ImmediateUserInterfacePlotXYSettings_RenderLabels;

                if(m_UI->begin_scrollarea(m_UI->next_id("Plot"),
                      Frenchie::Application::ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment
                    | Frenchie::Application::ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar
                    | Frenchie::Application::ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar))
                {
                    Frenchie::Application::ImmediateUserInterfacePlotAxis              axis(MinAxis, MaxAxis);
                    std::vector<Frenchie::Application::ImmediateUserInterfacePlotData> data;

                    for (int i = 0; i < (int)XAxisValues.size(); i++)
                    {
                        data.push_back(Frenchie::Application::ImmediateUserInterfacePlotData(
                            &XAxisValues[i][0],
                            &YAxisValues[i][0],
                            (int)XAxisValues[i].size(),
                            LineColors[i],
                            12.f,
                            PlotMeta.MinValue,
                            PlotMeta.MaxValue));
                    }

                    PlotMeta = m_UI->plotXY(m_UI->next_id("PlotXY"), axis, data, plotSettings);
                    m_UI->end_scrollarea();
                }

                m_UI->next_content_margin(m_UI->get_content_default_margin());
                m_UI->next_content_padding(m_UI->get_content_default_margin());

                if(m_UI->begin_scrollarea(
                    m_UI->next_id("Settings"),
                    Frenchie::Application::ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable
                    | Frenchie::Application::ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar
                    | Frenchie::Application::ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar))
                {
                    // checkboxes
                    m_UI->check_button(m_UI->next_id("EditableChekbox"), Editable);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("Editable"), "Editable");
                    
                    m_UI->check_button(m_UI->next_id("ZoomableChekbox"), Zoomable);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("Zoomable"), "Zoomable");
                    
                    m_UI->check_button(m_UI->next_id("DraggableChekbox"), Draggable);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("Draggable"), "Draggable");
                    
                    m_UI->check_button(m_UI->next_id("RenderPointsChekbox"), RenderPoints);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("RenderPoints"), "RenderPoints");
                    
                    m_UI->check_button(m_UI->next_id("RenderLabelsChekbox"), RenderLabels);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("RenderLabels"), "RenderLabels");

                    // regulators

                    // Xmin
                    m_UI->input_scalar(m_UI->next_id("XminValue"), MinAxis.x, -100.f, +100.f);
                    m_UI->same_line();
                    m_UI->input_scalar_slider(m_UI->next_id("XminXSilder"), MinAxis.x, -100.f, +100.f);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("Xmin"), "Xmin");

                    // Xmax
                    m_UI->input_scalar(m_UI->next_id("XmaxValue"), MaxAxis.x, -100.f, +100.f);
                    m_UI->same_line();
                    m_UI->input_scalar_slider(m_UI->next_id("XmaxSilder"), MaxAxis.x, -100.f, +100.f);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("Xmax"), "Xmax");
                    
                    // Ymin
                    m_UI->input_scalar(m_UI->next_id("YminValue"), MinAxis.y, -2.f, +2.f);
                    m_UI->same_line();
                    m_UI->input_scalar_slider(m_UI->next_id("YminSlider"), MinAxis.y, -2.f, +2.f);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("Ymin"), "Ymin");

                    // Ymax
                    m_UI->input_scalar(m_UI->next_id("YmaxValue"), MaxAxis.y, -2.f, +2.f);
                    m_UI->same_line();
                    m_UI->input_scalar_slider(m_UI->next_id("YmaxSlider"), MaxAxis.y, -2.f, +2.f);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("Ymax"), "Ymax");

                    m_UI->end_scrollarea();
                }

                m_UI->end_horizontal_stack();
            }

            m_UI->end_window();
        }
    }

    std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceContextLayer> m_UI{nullptr};

    Frenchie::Application::ImmediateUserInterfacePlotMeta PlotMeta;
    std::vector<std::vector<float>>                       XAxisValues;
    std::vector<std::vector<float>>                       YAxisValues;
    std::vector<gs_color>                                 LineColors;
    gs_vec2f                                              MinAxis;
    gs_vec2f                                              MaxAxis;

    bool Editable     {true};
    bool Zoomable     {true};
    bool Draggable    {true};
    bool RenderPoints {true};
    bool RenderLabels {true};
};

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    Frenchie::Application::application()->push_layer<SomeSimpleGuiLayer>();
    return Frenchie::Application::application()->execute();
}