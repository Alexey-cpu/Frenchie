#include <FrenchieImmediateUserInterfaceStyleTest.hpp>

using namespace Frenchie::Application;

FrenchieImmediateUserInterfaceStyleTest::FrenchieImmediateUserInterfaceStyleTest() : Layer(STRINGIFY(FrenchieImmediateUserInterfaceStyleTest)){}
FrenchieImmediateUserInterfaceStyleTest::~FrenchieImmediateUserInterfaceStyleTest(){}

bool FrenchieImmediateUserInterfaceStyleTest::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::application()->push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();
    return m_UI != nullptr;
}

void FrenchieImmediateUserInterfaceStyleTest::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("Interface style window", "InterfaceStyleWindow"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        if(m_UI->begin_scrollarea(m_UI->next_id("ScrollArea")))
        {
            m_UI->label(m_UI->next_id("GeometrySettings"), "Geometry settings");

            // font size
            m_UI->input_scalar_slider(m_UI->next_id("FontSize"), m_UI->m_Style.get_font_size(), m_UI->m_Style.get_minimum_font_size(), m_UI->m_Style.get_maximum_font_size(), 1);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("FontSizeLabel"), "Font size");

            // frames radius
            m_UI->input_scalar_slider(m_UI->next_id("FramesRadius"), m_UI->m_Style.get_frames_radius(), m_UI->m_Style.get_minimum_frames_radius(), m_UI->m_Style.get_maximum_frames_radius(), 1);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("FramesRadiusLabel"), "Frames radius");

            // frames width
            m_UI->input_scalar_slider(m_UI->next_id("FramesWidth"), m_UI->m_Style.get_frames_width(), m_UI->m_Style.get_minimum_frames_width(), m_UI->m_Style.get_maximum_frames_width(), 1);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("FramesWidthLabel"), "Frames width");

            // scrollbar width
            m_UI->input_scalar_slider(m_UI->next_id("ScrollBarWidth"), m_UI->m_Style.get_scrollbar_width(), m_UI->m_Style.get_minimum_scrollbar_width(), m_UI->m_Style.get_maximum_scrollbar_width(), 1);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("ScrollBarWidthLabel"), "Scroll bar width");

            m_UI->next_line();
            m_UI->next_line();

            m_UI->label(m_UI->next_id("ColorScheme"), "Color scheme");

            for (int i = ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Begin;
                     i < ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_End;
                     i++)
            {
                if(m_UI->input_color(
                    m_UI->next_id(Frenchie::Core::String::format("Color-%d", i)),
                    m_UI->m_Style.get_color((ImmediateUserInterfaceNodeColors_)i),
                    ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB | ImmediateUserInterfaceColorPickerSettings_PreviewColor))
                {
                    m_ShowColorPciker  = true;
                    m_ColorPickerColor = i;
                }

                m_UI->same_line();
                m_UI->indent(32.f);

                m_UI->label(m_UI->next_id(Frenchie::Core::String::format("Label-%d", i)), m_UI->m_Style.style_color_to_string((ImmediateUserInterfaceNodeColors_)i));
            }

            m_UI->end_scrollarea();
        }

        m_UI->end_window();
    }

    if(m_UI->begin_dialog(
        m_UI->next_id("Color picker dialog", "ColorPicker"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults, &m_ShowColorPciker))
    {
        m_UI->next_maximum_size(gs_vec2f(gs_huge<float>(), m_UI->m_Style.get_font_size()));

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
                    m_UI->m_Style.get_color((ImmediateUserInterfaceNodeColors_)m_ColorPickerColor));
            }
            // HSVA
            else
            {
                m_UI->color_picker_hsva(
                    m_UI->next_id("HSVAColorPicker"),
                    m_UI->m_Style.get_color((ImmediateUserInterfaceNodeColors_)m_ColorPickerColor));
            }

            m_UI->end_horizontal_stack();
        }

        m_UI->end_dialog();
    }
}

bool FrenchieImmediateUserInterfaceStyleTest::allows_multiple_instances() const
{
    return false;
}