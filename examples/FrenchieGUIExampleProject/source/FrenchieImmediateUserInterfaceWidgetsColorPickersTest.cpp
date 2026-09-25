#include <FrenchieImmediateUserInterfaceWidgetsColorPickersTest.hpp>

using namespace Frenchie::Application;

FrenchieImmediateUserInterfaceWidgetsColorPickersTest::FrenchieImmediateUserInterfaceWidgetsColorPickersTest() : Layer(STRINGIFY(FrenchieImmediateUserInterfaceWidgetsColorPickersTest)){}
FrenchieImmediateUserInterfaceWidgetsColorPickersTest::~FrenchieImmediateUserInterfaceWidgetsColorPickersTest(){}

bool FrenchieImmediateUserInterfaceWidgetsColorPickersTest::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();
    return m_UI != nullptr;
}

void FrenchieImmediateUserInterfaceWidgetsColorPickersTest::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("Color pickers test window", "ColorPickersTestWindow"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        // settings
        if(m_UI->begin_scrollarea(
            m_UI->next_id("Settings"),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically))
        {
            m_UI->check_box(m_UI->next_id("EnableRGB"), m_EnableRGB);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("EnableRGBLabel"), "RGB");
            
            m_UI->same_line();
            m_UI->indent();
            m_UI->check_box(m_UI->next_id("EnableHSV"), m_EnableHSV);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("EnableHSVLabel"), "HSV");

            m_UI->same_line();
            m_UI->indent();
            m_UI->check_box(m_UI->next_id("EnableHSL"), m_EnableHSL);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("EnableHSLLabel"), "HSL");

            m_UI->same_line();
            m_UI->indent();
            m_UI->check_box(m_UI->next_id("PreviewColor"), m_PreviewColor);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("PreviewColorLabel"), "Preview color");

            m_UI->same_line();
            m_UI->indent();
            m_UI->check_box(m_UI->next_id("EnableAlpha"), m_EnableAlpha);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("EnableAlphaLabel"), "Alpha");
            m_UI->indent();
            m_UI->same_line();

            if(m_UI->begin_combobox(m_UI->next_id("Combobox"), m_RGBA ? "RGBA" : "HSVA"))
            {
                bool rgbaSelected     = m_RGBA;
                bool hsvaSelected     = !m_RGBA;
                int  checkboxSettings = ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox;

                m_UI->check_box(m_UI->next_id("RGBASelected"), rgbaSelected, checkboxSettings);
                m_UI->same_line();
                if(m_UI->combobox_item(m_UI->next_id("RGBA", "RGBA"))) m_RGBA = true;

                m_UI->check_box(m_UI->next_id("HSVASelected"), hsvaSelected, checkboxSettings);
                m_UI->same_line();
                if(m_UI->combobox_item(m_UI->next_id("HSVA", "HSVA"))) m_RGBA = false;

                m_UI->end_combobox();
            }

            m_UI->end_scrollarea();
        }

        if(m_UI->begin_horizontal_stack(m_UI->next_id("ColorPickers")))
        {
            // RGBA
            if(m_RGBA)
            {
                m_UI->color_picker_rgba(
                    m_UI->next_id("RGBAColorPicker"),
                    m_ColorPickerColor,
                      (m_EnableRGB    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB          : 0)
                    | (m_EnableHSV    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV            : 0)
                    | (m_EnableHSL    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL            : 0)
                    | (m_EnableAlpha  ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha          : 0)
                    | (m_PreviewColor ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorButton : 0)
                );
            }
            // HSVA
            else
            {
                m_UI->color_picker_hsva(
                    m_UI->next_id("HSVAColorPicker"),
                    m_ColorPickerColor,
                      (m_EnableRGB    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB          : 0)
                    | (m_EnableHSV    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV            : 0)
                    | (m_EnableHSL    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL            : 0)
                    | (m_EnableAlpha  ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha          : 0)
                    | (m_PreviewColor ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorButton : 0)
                );
            }

            m_UI->end_horizontal_stack();
        }

        m_UI->end_window();
    }
}

bool FrenchieImmediateUserInterfaceWidgetsColorPickersTest::allows_multiple_instances() const
{
    return false;
}