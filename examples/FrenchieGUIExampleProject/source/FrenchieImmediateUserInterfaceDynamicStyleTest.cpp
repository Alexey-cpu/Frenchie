#include <FrenchieImmediateUserInterfaceDynamicStyleTest.hpp>

#include <FrenchieImmediateUserInterfaceDynamicStyleTest.hpp>

using namespace Frenchie::Application;

FrenchieImmediateUserInterfaceDynamicStyleTest::FrenchieImmediateUserInterfaceDynamicStyleTest() : Layer(STRINGIFY(FrenchieImmediateUserInterfaceDynamicStyleTest)){}
FrenchieImmediateUserInterfaceDynamicStyleTest::~FrenchieImmediateUserInterfaceDynamicStyleTest(){}

bool FrenchieImmediateUserInterfaceDynamicStyleTest::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();
    return m_UI != nullptr;
}

void FrenchieImmediateUserInterfaceDynamicStyleTest::frame_update()
{
    m_UI->next_style(m_Style);

    if(m_UI->begin_window(
        m_UI->next_id("Dynamic interface style window", "DynamicInterfaceStyleWindow"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        if(m_UI->begin_vertical_stack(m_UI->next_id("Root")))
        {
            m_UI->next_content_margin(m_UI->get_content_default_margin());

            if(m_UI->begin_scrollarea(
                m_UI->next_id("Geometry"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically))
            {
                m_UI->label(m_UI->next_id("GeometrySettings"), "Geometry settings");

                // font size
                m_UI->input_scalar_slider(m_UI->next_id("FontSizeSlider"), m_Style.get_font_size(), m_Style.get_minimum_font_size(), m_Style.get_maximum_font_size(), 1);
                m_UI->same_line();
                m_UI->input_scalar(m_UI->next_id("FontSizeInput"), m_Style.get_font_size(), m_Style.get_minimum_font_size(), m_Style.get_maximum_font_size(), 1);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("FontSizeLabel"), "Font size");

                // frames radius
                m_UI->input_scalar_slider(m_UI->next_id("FramesRadiusSlider"), m_Style.get_frames_radius(), m_Style.get_minimum_frames_radius(), m_Style.get_maximum_frames_radius(), 1);
                m_UI->same_line();
                m_UI->input_scalar(m_UI->next_id("FramesRadiusInput"), m_Style.get_frames_radius(), m_Style.get_minimum_frames_radius(), m_Style.get_maximum_frames_radius(), 1);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("FramesRadiusLabel"), "Frames radius");

                // frames width
                m_UI->input_scalar_slider(m_UI->next_id("FramesWidthSlider"), m_Style.get_frames_width(), m_Style.get_minimum_frames_width(), m_Style.get_maximum_frames_width(), 1);
                m_UI->same_line();
                m_UI->input_scalar(m_UI->next_id("FramesWidthInput"), m_Style.get_frames_width(), m_Style.get_minimum_frames_width(), m_Style.get_maximum_frames_width(), 1);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("FramesWidthLabel"), "Frames width");

                // scrollbar width
                m_UI->input_scalar_slider(m_UI->next_id("ScrollBarWidthSlider"), m_Style.get_scrollbar_width(), m_Style.get_minimum_scrollbar_width(), m_Style.get_maximum_scrollbar_width(), 1);
                m_UI->same_line();
                m_UI->input_scalar(m_UI->next_id("ScrollBarWidthInput"), m_Style.get_scrollbar_width(), m_Style.get_minimum_scrollbar_width(), m_Style.get_maximum_scrollbar_width(), 1);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("ScrollBarWidthLabel"), "Scroll bar width");

                // filler
                m_UI->next_line();
                m_UI->next_size(gs_vec2f(0.f, m_UI->get_text_line_height()));
                m_UI->empty_node(m_UI->next_id("Filler"));

                m_UI->end_scrollarea();
            }

            m_UI->next_content_margin(m_UI->get_content_default_margin());

            if(m_UI->begin_scrollarea(m_UI->next_id("ColorScheme")))
            {
                for (int color = ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Begin;
                         color < ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_End;
                         color++)
                {
                    if(m_UI->input_color(
                        m_UI->next_id(Frenchie::Core::String::format("Color-%d", color)),
                        m_Style.get_color((ImmediateUserInterfaceNodeColors_)color),
                          ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB
                        | ImmediateUserInterfaceColorPickerSettings_PreviewColorButton))
                    {
                        m_ShowColorPciker  = true;
                        m_ColorPickerColor = color;
                    }

                    m_UI->same_line();
                    m_UI->indent(32.f);

                    m_UI->label(m_UI->next_id(Frenchie::Core::String::format("Label-%d", color)), m_Style.style_color_to_string((ImmediateUserInterfaceNodeColors_)color));
                }

                m_UI->end_scrollarea();
            }

            m_UI->end_vertical_stack();
        }

        m_UI->end_window();
    }

    if(m_UI->begin_dialog(
        m_UI->next_id("Color picker dialog", "ColorPicker"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults, &m_ShowColorPciker))
    {
        m_UI->next_content_margin(m_UI->get_content_default_margin());

        if(m_UI->begin_vertical_stack(
            m_UI->next_id("ColorEditor"),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter))
        {
            m_UI->next_maximum_size(gs_vec2f(gs_huge<float>(), m_UI->get_text_line_height()));

            if(m_UI->begin_horizontal_stack(m_UI->next_id("Combobox")))
            {
                m_UI->label(m_UI->next_id("ColorPickerType"), "Type");

                if(m_UI->begin_combobox(m_UI->next_id("Combobox"),m_RGBAColorPicker ? "RGBA" : "HSVA"))
                {
                    bool rgbaSelected     = m_RGBAColorPicker;
                    bool hsvaSelected     = !m_RGBAColorPicker;
                    int  checkboxSettings = ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox;

                    m_UI->check_button(m_UI->next_id("RGBASelected"), rgbaSelected, checkboxSettings);
                    m_UI->same_line();
                    if(m_UI->combobox_item(m_UI->next_id("RGBA", "RGBA"))) m_RGBAColorPicker = true;

                    m_UI->check_button(m_UI->next_id("HSVASelected"), hsvaSelected, checkboxSettings);
                    m_UI->same_line();
                    if(m_UI->combobox_item(m_UI->next_id("HSVA", "HSVA"))) m_RGBAColorPicker = false;

                    m_UI->end_combobox();
                }

                m_UI->end_horizontal_stack();
            }

            if(m_UI->begin_horizontal_stack(m_UI->next_id("Pickers")))
            {
                // RGBA
                if(m_RGBAColorPicker)
                {
                    m_UI->color_picker_rgba(
                        m_UI->next_id("RGBAColorPicker"),
                        m_Style.get_color((ImmediateUserInterfaceNodeColors_)m_ColorPickerColor));
                }
                // HSVA
                else
                {
                    m_UI->color_picker_hsva(
                        m_UI->next_id("HSVAColorPicker"),
                        m_Style.get_color((ImmediateUserInterfaceNodeColors_)m_ColorPickerColor));
                }

                m_UI->end_horizontal_stack();
            }

            m_UI->end_vertical_stack();
        }

        m_UI->end_dialog();
    }
}

bool FrenchieImmediateUserInterfaceDynamicStyleTest::allows_multiple_instances() const
{
    return false;
}