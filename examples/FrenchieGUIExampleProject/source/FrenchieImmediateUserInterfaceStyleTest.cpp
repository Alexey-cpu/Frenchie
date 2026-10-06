#include <FrenchieImmediateUserInterfaceStyleTest.hpp>

// STL
#include <filesystem>
#include <iostream>

using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        class ApplicationFonts : public Frenchie::Application::Layer
        {
        public:
            ApplicationFonts() : Frenchie::Application::Layer(STRINGIFY(ApplicationFonts)){}
            virtual ~ApplicationFonts(){}

            virtual bool awake() override
            {
                std::filesystem::path fontsPath(std::filesystem::current_path().u32string().append(U"/assets/fonts/"));

                if(std::filesystem::exists(fontsPath))
                {
                    for (const auto& entry : std::filesystem::recursive_directory_iterator(fontsPath, std::filesystem::directory_options::skip_permission_denied))
                    {
                        if(!entry.is_directory() && (entry.path().extension().stem() == ".ttf" || entry.path().extension().stem() == ".otf"))
                        {
                            std::cout << "loading font " << entry.path().filename().stem().string() << "\n";

                            m_Fonts[entry.path().filename().stem().string()] =
                                ApplicationRenderingBackend::construct_font(entry.path().string().c_str(), 32);
                        }
                    }
                }
                else
                    std::cout << "path " << fontsPath << " does not exist \n";

                return true;
            }

            virtual void finish() override
            {
                for(auto& font : m_Fonts)
                    ApplicationRenderingBackend::destroy_font(font.second);
                m_Fonts.clear();
            }
            
            virtual bool allows_multiple_instances() const override
            {
                return false;
            }

            std::map<std::string, ApplicationRenderingBackendFont> m_Fonts {std::map<std::string, ApplicationRenderingBackendFont>()};
        };
    }
}

FrenchieImmediateUserInterfaceStyleTest::FrenchieImmediateUserInterfaceStyleTest() : Layer(STRINGIFY(FrenchieImmediateUserInterfaceStyleTest)){}
FrenchieImmediateUserInterfaceStyleTest::~FrenchieImmediateUserInterfaceStyleTest(){}

bool FrenchieImmediateUserInterfaceStyleTest::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

    return m_UI != nullptr;
}

void FrenchieImmediateUserInterfaceStyleTest::frame_update()
{
    std::shared_ptr<Frenchie::Application::ApplicationFonts> appFonts = Frenchie::Application::App::push_layer<Frenchie::Application::ApplicationFonts>();
    
    if(m_UI->begin_window(
        m_UI->next_id("Interface style window", "InterfaceStyleWindow"),
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

                // font
                std::string comboPreview = "Default";

                for(auto font : appFonts->m_Fonts)
                {
                    if(m_UI->style().get_current_font() == font.second)
                        comboPreview = font.first;
                }

                if(m_UI->begin_combobox(m_UI->next_id("Fonts"), comboPreview))
                {
                    if(m_UI->combobox_item(m_UI->next_id("Default", "Default")))
                        m_UI->style().get_current_font() = ApplicationRenderingBackend::get_default_font();

                    for(auto font : appFonts->m_Fonts)
                    {
                        if(m_UI->combobox_item(m_UI->next_id(font.first, font.first)))
                            m_UI->style().get_current_font() = font.second;
                    }

                    m_UI->end_combobox();
                }

                m_UI->same_line();

                m_UI->label(m_UI->next_id("CurrentFont"), "Current font");
                m_UI->next_line();
                m_UI->next_line();

                // font size
                m_UI->input_scalar_slider(m_UI->next_id("FontSizeSlider"), m_UI->style().get_font_size(), m_UI->style().get_minimum_font_size(), m_UI->style().get_maximum_font_size(), 1);
                m_UI->same_line();
                m_UI->input_scalar(m_UI->next_id("FontSizeInput"), m_UI->style().get_font_size(), m_UI->style().get_minimum_font_size(), m_UI->style().get_maximum_font_size(), 1);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("FontSizeLabel"), "Font size");

                // frames radius
                m_UI->input_scalar_slider(m_UI->next_id("FramesRadiusSlider"), m_UI->style().get_frames_radius(), m_UI->style().get_minimum_frames_radius(), m_UI->style().get_maximum_frames_radius(), 1);
                m_UI->same_line();
                m_UI->input_scalar(m_UI->next_id("FramesRadiusInput"), m_UI->style().get_frames_radius(), m_UI->style().get_minimum_frames_radius(), m_UI->style().get_maximum_frames_radius(), 1);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("FramesRadiusLabel"), "Frames radius");

                // frames width
                m_UI->input_scalar_slider(m_UI->next_id("FramesWidthSlider"), m_UI->style().get_frames_width(), m_UI->style().get_minimum_frames_width(), m_UI->style().get_maximum_frames_width(), 1);
                m_UI->same_line();
                m_UI->input_scalar(m_UI->next_id("FramesWidthInput"), m_UI->style().get_frames_width(), m_UI->style().get_minimum_frames_width(), m_UI->style().get_maximum_frames_width(), 1);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("FramesWidthLabel"), "Frames width");

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
                        m_UI->style().get_color((ImmediateUserInterfaceNodeColors_)color),
                          ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB
                        | ImmediateUserInterfaceColorPickerSettings_PreviewColorButton))
                    {
                        m_ShowColorPciker  = true;
                        m_ColorPickerColor = color;
                    }

                    m_UI->same_line();
                    m_UI->indent(32.f);

                    m_UI->label(m_UI->next_id(Frenchie::Core::String::format("Label-%d", color)), m_UI->style().style_color_to_string((ImmediateUserInterfaceNodeColors_)color));
                }

                m_UI->end_scrollarea();
            }

            m_UI->end_vertical_stack();
        }

        m_UI->end_window();
    }

    if(m_UI->begin_dialog(
        m_UI->next_id("Color picker dialog", "ColorPicker"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults
        | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ShowDialogBlur, &m_ShowColorPciker))
    {
        m_UI->next_content_margin(m_UI->get_content_default_margin());

        if(m_UI->begin_vertical_stack(
            m_UI->next_id("ColorEditor"),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter))
        {
            m_UI->next_height(m_UI->get_text_line_height() * 2.f);

            if(m_UI->begin_horizontal_stack(m_UI->next_id("Combobox")))
            {
                if(m_UI->begin_combobox(m_UI->next_id("Combobox"),m_RGBAColorPicker ? "RGBA" : "HSVA"))
                {
                    bool rgbaSelected     = m_RGBAColorPicker;
                    bool hsvaSelected     = !m_RGBAColorPicker;
                    int  checkboxSettings = ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox;

                    m_UI->check_box(m_UI->next_id("RGBASelected"), rgbaSelected, checkboxSettings);
                    m_UI->same_line();
                    if(m_UI->combobox_item(m_UI->next_id("RGBA", "RGBA"))) m_RGBAColorPicker = true;

                    m_UI->check_box(m_UI->next_id("HSVASelected"), hsvaSelected, checkboxSettings);
                    m_UI->same_line();
                    if(m_UI->combobox_item(m_UI->next_id("HSVA", "HSVA"))) m_RGBAColorPicker = false;

                    m_UI->end_combobox();
                }

                m_UI->label(m_UI->next_id("ColorPickerType"), "Type");

                m_UI->end_horizontal_stack();
            }

            if(m_UI->begin_horizontal_stack(m_UI->next_id("Pickers")))
            {
                // RGBA
                if(m_RGBAColorPicker)
                {
                    m_UI->color_picker_rgba(
                        m_UI->next_id("RGBAColorPicker"),
                        m_UI->style().get_color((ImmediateUserInterfaceNodeColors_)m_ColorPickerColor));
                }
                // HSVA
                else
                {
                    m_UI->color_picker_hsva(
                        m_UI->next_id("HSVAColorPicker"),
                        m_UI->style().get_color((ImmediateUserInterfaceNodeColors_)m_ColorPickerColor));
                }

                m_UI->end_horizontal_stack();
            }

            m_UI->end_vertical_stack();
        }

        m_UI->end_dialog();
    }
}

void FrenchieImmediateUserInterfaceStyleTest::finish()
{
}

bool FrenchieImmediateUserInterfaceStyleTest::allows_multiple_instances() const
{
    return false;
}