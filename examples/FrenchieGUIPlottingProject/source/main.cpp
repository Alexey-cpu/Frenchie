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

            for (int n = 0; n < (int)(T * fs); n++)
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
            if(m_UI->begin_vertical_stack(m_UI->next_id("Plot")))
            {
                if(m_UI->begin_panel(m_UI->next_id("Plot")))
                {
                    Frenchie::Application::ImmediateUserInterfacePlotAxis              axis(gs_vec2f(minX, minY), gs_vec2f(maxX, maxY));
                    std::vector<Frenchie::Application::ImmediateUserInterfacePlotData> data;

                    for (int i = 0; i < (int)XX.size(); i++)
                        data.push_back(Frenchie::Application::ImmediateUserInterfacePlotData(&XX[i][0], &YY[i][0], (int)XX[i].size(), CC[i]));

                    m_UI->plotXY(m_UI->next_id("PlotXY"), axis, data);

                    m_UI->end_panel();
                }

                m_UI->next_content_margin(m_UI->get_content_default_margin());

                if(m_UI->begin_vertical_stack(
                    m_UI->next_id("Settings"),
                    Frenchie::Application::ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable
                    | Frenchie::Application::ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter
                    | Frenchie::Application::ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter))
                {
                    int alignment =
                      Frenchie::Application::ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter
                    | Frenchie::Application::ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter;

                    gs_vec4f padding = gs_vec4f(0.f, 0.f, 32.f, 0.f);

                    // buttons
                    m_UI->next_content_padding(padding);

                    auto parentSize = m_UI->current_bounding_box(m_UI->get_rendering_stack_top()).size();

                    m_UI->next_size(gs_vec2f(parentSize.x, parentSize.y * 0.25f));

                    if(m_UI->begin_horizontal_stack(m_UI->next_id("Buttons"), alignment))
                    {
                        m_UI->push_button(m_UI->next_id("Reset zoom", "ResetZoom"));
                        m_UI->push_button(m_UI->next_id("Reset offset", "ResetOffset"));

                        m_UI->end_horizontal_stack();
                    }

                    // grid
                    if(m_UI->begin_grid(m_UI->next_id("Settings"), alignment))
                    {
                        // Xmin
                        if(m_UI->begin_grid_place(0, 0, alignment))
                        {
                            m_UI->label(m_UI->next_id("Xmin"), "Xmin");
                            m_UI->end_grid_place();
                        }

                        if(m_UI->begin_grid_place(0, 1, alignment))
                        {
                            m_UI->next_content_padding(padding);

                            if(m_UI->begin_horizontal_stack(m_UI->next_id("Xmin"), alignment))
                            {
                                m_UI->next_size(m_UI->current_bounding_box(m_UI->get_rendering_stack_top()).size() * 0.4f);
                                m_UI->input_scalar(m_UI->next_id("XminValue"), minX, -100.f, +100.f);

                                m_UI->next_size(m_UI->current_bounding_box(m_UI->get_rendering_stack_top()).size() * 0.4f);
                                m_UI->input_scalar_slider(m_UI->next_id("XminXSilder"), minX, -100.f, +100.f);
                                m_UI->end_horizontal_stack();
                            }

                            m_UI->end_grid_place();
                        }
                        
                        // Xmax
                        if(m_UI->begin_grid_place(1, 0, alignment))
                        {
                            m_UI->label(m_UI->next_id("Xmax"), "Xmax");
                            m_UI->end_grid_place();
                        }

                        if(m_UI->begin_grid_place(1, 1, alignment))
                        {
                            m_UI->next_content_padding(padding);

                            if(m_UI->begin_horizontal_stack(m_UI->next_id("Xmax"), alignment))
                            {
                                m_UI->next_size(m_UI->current_bounding_box(m_UI->get_rendering_stack_top()).size() * 0.4f);
                                m_UI->input_scalar(m_UI->next_id("XmaxValue"), maxX, -100.f, +100.f);

                                m_UI->next_size(m_UI->current_bounding_box(m_UI->get_rendering_stack_top()).size() * 0.4f);
                                m_UI->input_scalar_slider(m_UI->next_id("XmaxSilder"), maxX, -100.f, +100.f);
                                m_UI->end_horizontal_stack();
                            }

                            m_UI->end_grid_place();
                        }

                        // Ymin
                        if(m_UI->begin_grid_place(2, 0, alignment))
                        {
                            m_UI->label(m_UI->next_id("Ymin"), "Ymin");
                            m_UI->end_grid_place();
                        }

                        if(m_UI->begin_grid_place(2, 1, alignment))
                        {
                            m_UI->next_content_padding(padding);

                            if(m_UI->begin_horizontal_stack(m_UI->next_id("Ymin"), alignment))
                            {
                                m_UI->next_size(m_UI->current_bounding_box(m_UI->get_rendering_stack_top()).size() * 0.4f);
                                m_UI->input_scalar(m_UI->next_id("YminValue"), minY, -2.f, +2.f);

                                m_UI->next_size(m_UI->current_bounding_box(m_UI->get_rendering_stack_top()).size() * 0.4f);
                                m_UI->input_scalar_slider(m_UI->next_id("YminSlider"), minY, -2.f, +2.f);
                                m_UI->end_horizontal_stack();
                            }

                            m_UI->end_grid_place();
                        }

                        // Ymax
                        if(m_UI->begin_grid_place(3, 0, alignment))
                        {
                            m_UI->label(m_UI->next_id("Ymax"), "Ymax");
                            m_UI->end_grid_place();
                        }

                        if(m_UI->begin_grid_place(3, 1, alignment))
                        {
                            m_UI->next_content_padding(padding);

                            if(m_UI->begin_horizontal_stack(m_UI->next_id("Ymax"), alignment))
                            {
                                m_UI->next_size(m_UI->current_bounding_box(m_UI->get_rendering_stack_top()).size() * 0.4f);
                                m_UI->input_scalar(m_UI->next_id("YmaxValue"), minY, maxY, -2.f, +2.f);

                                m_UI->next_size(m_UI->current_bounding_box(m_UI->get_rendering_stack_top()).size() * 0.4f);
                                m_UI->input_scalar_slider(m_UI->next_id("YmaxSlider"), maxY, -2.f, +2.f);
                                m_UI->end_horizontal_stack();
                            }

                            m_UI->end_grid_place();
                        }

                        m_UI->end_grid();
                    }

                    m_UI->end_vertical_stack();
                }

                m_UI->end_vertical_stack();
            }

            m_UI->end_window();
        }
    }

    std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceContextLayer> m_UI{nullptr};

    std::vector<std::vector<float>> XX;
    std::vector<std::vector<float>> YY;
    std::vector<gs_color>           CC;
};

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    Frenchie::Application::application()->push_layer<SomeSimpleGuiLayer>();
    return Frenchie::Application::application()->execute();
}