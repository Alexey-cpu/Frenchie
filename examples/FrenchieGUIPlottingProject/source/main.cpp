#include <FrenchieApplication.hpp>
#include <FrenchieImmediateUserInterfaceLayer.hpp>

#include <iostream>

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
        float fn  = 50.f;
        float Ns  = 80.f;
        float fs = fn * Ns;  // Hs
        float Ts = 1.f / fs; // s
        float T  = 20.f / 1000.f;
        
        for (int n = 0; n < (int)(T * fs); n++)
        {
            Y.push_back(sin(PI2 * fn * Ts * (float)n));
            X.push_back(n);
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
            if(m_UI->begin_vertical_stack(m_UI->next_id("Plot")))
            {
                if(m_UI->begin_panel(m_UI->next_id("Plot")))
                {
                    m_UI->plot(
                        m_UI->next_id("PlotXY"),
                        &X[0],
                        &Y[0],
                        Y.size(),
                        minX,
                        maxX,
                        minY,
                        maxY);

                    m_UI->end_panel();
                }

                m_UI->next_content_margin(m_UI->get_content_default_margin());

                if(m_UI->begin_panel(
                    m_UI->next_id("Settings"),
                    Frenchie::Application::ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable
                    | Frenchie::Application::ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter
                    | Frenchie::Application::ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter))
                {
                    if(m_UI->begin_scrollarea(m_UI->next_id("Settings")))
                    {
                        // Xmin
                        m_UI->label(m_UI->next_id("MinXLabel"), "Xmin");
                        m_UI->same_line();
                        m_UI->input_scalar(m_UI->next_id("MinXValue"), minX, -100.f, +100.f);
                        m_UI->same_line();
                        m_UI->input_scalar_slider(m_UI->next_id("MinXSilder"), minX, -100.f, +100.f);

                        m_UI->label(m_UI->next_id("MaxXLabel"), "Xmax");
                        m_UI->same_line();
                        m_UI->input_scalar(m_UI->next_id("MaxXValue"), maxX, -100.f, +100.f);
                        m_UI->same_line();
                        m_UI->input_scalar_slider(m_UI->next_id("MaxXSilder"), maxX, -100.f, +100.f);

                        m_UI->label(m_UI->next_id("MinYLabel"), "Ymin");
                        m_UI->same_line();
                        m_UI->input_scalar(m_UI->next_id("MinYValue"), minY, -100.f, +100.f);
                        m_UI->same_line();
                        m_UI->input_scalar_slider(m_UI->next_id("MinYSilder"), minY, -2.f, +2.f);

                        m_UI->label(m_UI->next_id("MaxYLabel"), "maxY");
                        m_UI->same_line();
                        m_UI->input_scalar(m_UI->next_id("MaxYValue"), maxY, -2.f, +2.f);
                        m_UI->same_line();
                        m_UI->input_scalar_slider(m_UI->next_id("MaxYSilder"), maxY, -2.f, +2.f);

                        m_UI->end_scrollarea();
                    }

                    m_UI->end_panel();
                }

                m_UI->end_vertical_stack();
            }

            m_UI->end_window();
        }
    }

    std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceContextLayer> m_UI{nullptr};

    std::vector<float> X;
    std::vector<float> Y;
};

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    Frenchie::Application::application()->push_layer<SomeSimpleGuiLayer>();
    return Frenchie::Application::application()->execute();
}