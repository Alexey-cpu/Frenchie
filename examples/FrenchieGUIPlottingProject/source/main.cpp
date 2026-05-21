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
        
        for (int i = 0; i < 5; i++)
        {
            std::vector<float> X;
            std::vector<float> Y;
            float              A = gs_pseudo_random<float>(0.1f, 1.f);

            for (int n = 0; n <= (int)(T * fs); n++)
            {
                Y.push_back(A * sin(PI2 * fn * Ts * (float)n + gs_to_radians(i * 30.f)));
                X.push_back(n);
            }

            XX.push_back(X);
            YY.push_back(Y);
            CC.push_back(gs_color_rgb(gs_pseudo_random<int>(0, 255), gs_pseudo_random<int>(0, 255), gs_pseudo_random<int>(0, 255)));
        }


        return m_UI != nullptr;
    }

    virtual void frame_update() override
    {
        static float minX = 0.f;
        static float maxX = 100.f;
        static float minY = -1.f;
        static float maxY = +1.f;

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

                if(m_UI->begin_panel(m_UI->next_id("Plot")))
                {
                    Frenchie::Application::ImmediateUserInterfacePlotAxis              axis(gs_vec2f(minX, minY), gs_vec2f(maxX, maxY));
                    std::vector<Frenchie::Application::ImmediateUserInterfacePlotData> data;

                    for (int i = 0; i < (int)XX.size(); i++)
                        data.push_back(Frenchie::Application::ImmediateUserInterfacePlotData(&XX[i][0], &YY[i][0], (int)XX[i].size(), CC[i]));

                    m_UI->plotXY(m_UI->next_id("PlotXY"), axis, data, plotSettings);
                    m_UI->end_panel();
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
                    m_UI->input_scalar(m_UI->next_id("XminValue"), minX, -100.f, +100.f);
                    m_UI->same_line();
                    m_UI->input_scalar_slider(m_UI->next_id("XminXSilder"), minX, -100.f, +100.f);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("Xmin"), "Xmin");

                    // Xmax
                    m_UI->input_scalar(m_UI->next_id("XmaxValue"), maxX, -100.f, +100.f);
                    m_UI->same_line();
                    m_UI->input_scalar_slider(m_UI->next_id("XmaxSilder"), maxX, -100.f, +100.f);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("Xmax"), "Xmax");
                    
                    // Ymin
                    m_UI->input_scalar(m_UI->next_id("YminValue"), minY, -2.f, +2.f);
                    m_UI->same_line();
                    m_UI->input_scalar_slider(m_UI->next_id("YminSlider"), minY, -2.f, +2.f);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("Ymin"), "Ymin");

                    // Ymax
                    m_UI->input_scalar(m_UI->next_id("YmaxValue"), minY, maxY, -2.f, +2.f);
                    m_UI->same_line();
                    m_UI->input_scalar_slider(m_UI->next_id("YmaxSlider"), maxY, -2.f, +2.f);
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

    std::vector<std::vector<float>> XX;
    std::vector<std::vector<float>> YY;
    std::vector<gs_color>           CC;

    bool Editable     {true};
    bool Zoomable     {true};
    bool Draggable    {true};
    bool RenderPoints {true};
    bool RenderLabels {true};


    // ImmediateUserInterfacePlotXYSettings_None         = 0,      ///< sentinel
    // ImmediateUserInterfacePlotXYSettings_Editable     = 1 << 0, ///< enables XY plot points editing
    // ImmediateUserInterfacePlotXYSettings_Zoomable     = 1 << 1, ///< enables zoom
    // ImmediateUserInterfacePlotXYSettings_Draggable    = 1 << 2, ///< enables drag

    // ImmediateUserInterfacePlotXYSettings_RenderPoints = 1 << 3, ///< enables points rendering of a plot data
    // ImmediateUserInterfacePlotXYSettings_RenderLabels = 1 << 4, ///< enables labels rendering of a plot data when mouse hovers a point

};

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    Frenchie::Application::application()->push_layer<SomeSimpleGuiLayer>();
    return Frenchie::Application::application()->execute();
}