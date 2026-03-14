#include <ImmediateUserInterfaceTestLayer.hpp>

#include <FrenchieCoreStringUtilities.hpp>

using namespace Frenchie::Application;

ImmediateUserInterfaceTestLayer::ImmediateUserInterfaceTestLayer(){}
ImmediateUserInterfaceTestLayer::~ImmediateUserInterfaceTestLayer(){}

bool ImmediateUserInterfaceTestLayer::awake()
{
    if(m_ImmediateUserInterface == nullptr)
        m_ImmediateUserInterface = Frenchie::Application::application()->push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

    return true;
}

void ImmediateUserInterfaceTestLayer::frame_update()
{
    widgets_test();
    //windows_test();
    //scrollarea_test();
    
    //renderer_test();

    auto FPS = std::string("FPS ").append(std::to_string(m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().FrameRate));
}

void ImmediateUserInterfaceTestLayer::finish()
{
    if(m_ImmediateUserInterface != nullptr)
        m_ImmediateUserInterface->close();
}

void ImmediateUserInterfaceTestLayer::windows_test()
{
    m_ImmediateUserInterface->m_Settings =
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes |
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking |
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking;

    static bool opened = true;
    static std::string button;

    if(m_ImmediateUserInterface->begin_window("Metrics"))
    {
        m_ImmediateUserInterface->label(
            "Metrics/FPS",
            Frenchie::Core::String::format("Частота кадров %.4f", m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().FrameRate));

        m_ImmediateUserInterface->label(
            "Metrics/Commands",
            Frenchie::Core::String::format("Количество комманд рисовки %d", m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().RenderingCommandsCount));

        m_ImmediateUserInterface->label(
            "Metrics/Triangles",
            Frenchie::Core::String::format("Количество треугольников в сетке %d", m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().RenderedTrianglesCount));

        m_ImmediateUserInterface->push_button("Button-1###Metrics/Button-1");
        m_ImmediateUserInterface->push_button("Button-2###Metrics/Button-2");

        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window(
        "Window-1",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable  |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    {
        if(m_ImmediateUserInterface->begin_scrollarea(
            "Window-1/ScrollArea",
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults))
        {
            int k = 0;

            for(int i = 0 ; i < 1e1; i++)
            {
                for(int j = 0 ; j < 1e1; j++)
                    m_ImmediateUserInterface->push_button(std::string("Button-").append(std::to_string(k++)));

                m_ImmediateUserInterface->next_line();
            }

            m_ImmediateUserInterface->end_scrollarea();
        }

        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window("Какое-то окно=)###Window"))
    {
        if(m_ImmediateUserInterface->begin_menu("Menu-1"))
        {
            if(m_ImmediateUserInterface->begin_menu("Menu-2"))
            {
                static bool checked = false;
                m_ImmediateUserInterface->check_button("Checkbox", checked, ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox);
                m_ImmediateUserInterface->menu_action("Action-4");

                m_ImmediateUserInterface->check_button("RadioButton", checked, ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_RadioButton);
                m_ImmediateUserInterface->menu_action("Action-5");

                m_ImmediateUserInterface->check_button("SliderButton", checked, ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_SliderButton);
                m_ImmediateUserInterface->menu_action("Action-6");

                m_ImmediateUserInterface->menu_action("Action-7");
                m_ImmediateUserInterface->menu_action("Action-8");
                m_ImmediateUserInterface->menu_action("Action-9");
                m_ImmediateUserInterface->menu_action("Action-10");
                m_ImmediateUserInterface->menu_action("Action-11");
                m_ImmediateUserInterface->menu_action("Action-12");

                if(m_ImmediateUserInterface->begin_menu("Menu-333"))
                {
                    m_ImmediateUserInterface->menu_action("Action-333-1");
                    m_ImmediateUserInterface->menu_action("Action-333-22");
                    m_ImmediateUserInterface->menu_action("Action-333-333");
                    m_ImmediateUserInterface->menu_action("Action-333-4444");
                    m_ImmediateUserInterface->menu_action("Action-333-55555");

                    m_ImmediateUserInterface->end_menu();
                }

                m_ImmediateUserInterface->end_menu();
            }

            if(m_ImmediateUserInterface->begin_menu("Menu-3333"))
            {
                m_ImmediateUserInterface->menu_action("Action-3333-1");
                m_ImmediateUserInterface->menu_action("Action-3333-22");
                m_ImmediateUserInterface->menu_action("Action-3333-333");
                m_ImmediateUserInterface->menu_action("Action-3333-4444");
                m_ImmediateUserInterface->menu_action("Action-3333-55555");

                m_ImmediateUserInterface->end_menu();
            }

            m_ImmediateUserInterface->end_menu();
        }

        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window(
        "Window-2",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable, &opened))
    {
        // if(m_ImmediateUserInterface->begin_horizontal_stack("Window-2/Stack"))
        // {
        //     m_ImmediateUserInterface->push_button("Button-1###Window-2/Button-1");
        //     m_ImmediateUserInterface->push_button("Button-2###Window-2/Button-2");

        //     m_ImmediateUserInterface->end_horizontal_stack();
        // }

        m_ImmediateUserInterface->end_window();
    }

    // if(m_ImmediateUserInterface->begin_window(
    //     "Window-3",
    //     ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
    //     ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    // {
    //     m_ImmediateUserInterface->end_window();
    // }

    // if(m_ImmediateUserInterface->begin_window(
    //     "Window-4",
    //     ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
    //     ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    // {
    //     m_ImmediateUserInterface->end_window();
    // }

    // if(m_ImmediateUserInterface->begin_window(
    //     "Window-5",
    //     ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
    //     ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    // {        
    //     m_ImmediateUserInterface->end_window();
    // }

    // if(m_ImmediateUserInterface->begin_window(
    //     "Window-5###Window-6",
    //     ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
    //     ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    // {        
    //     m_ImmediateUserInterface->end_window();
    // }
}

void ImmediateUserInterfaceTestLayer::widgets_test()
{
    m_ImmediateUserInterface->m_Settings =
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking |
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes;

    /*
    // labels behaviour test
    if(m_ImmediateUserInterface->begin_window(m_ImmediateUserInterface->next_id("Нередактируемые текстовые поля", "Labels behaviour testing")))
    {
        m_ImmediateUserInterface->next_content_padding(gs_vec2f(16.f, 16.f));

        if(m_ImmediateUserInterface->begin_vertial_stack(m_ImmediateUserInterface->next_id("VerticalStack")))
        {
            m_ImmediateUserInterface->next_content_padding(gs_vec2f(16.f, 16.f));

            if(m_ImmediateUserInterface->begin_horizontal_stack(m_ImmediateUserInterface->next_id("HorizontalStackRow1")))
            {
                m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("UserName"), "Имя пользователя");
                m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("Password"), "Пароль");
                m_ImmediateUserInterface->end_horizontal_stack();
            }

            m_ImmediateUserInterface->next_content_padding(gs_vec2f(16.f, 16.f));

            if(m_ImmediateUserInterface->begin_horizontal_stack(m_ImmediateUserInterface->next_id("HorizontalStackRow2")))
            {
                m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("UserName"), "Имя пользователя");
                m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("Password"), "Пароль");
                m_ImmediateUserInterface->end_horizontal_stack();
            }

            m_ImmediateUserInterface->next_content_padding(gs_vec2f(16.f, 16.f));

            if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("ScrollArea")))
            {
                m_ImmediateUserInterface->label(
                    m_ImmediateUserInterface->next_id("SomeText"),
                    "Напишем здесь какой-то очень полезный\nмногострочный текст");

                m_ImmediateUserInterface->next_line();

                m_ImmediateUserInterface->label(
                    m_ImmediateUserInterface->next_id("SomeText1"),
                    "Напишем здесь какой-то очень полезный\nмногострочный текст");

                m_ImmediateUserInterface->end_scrollarea();
            }

            m_ImmediateUserInterface->end_vertical_stack();
        }

        m_ImmediateUserInterface->end_window();
    }

    // singleline text editor testing
    static std::string username;
    static std::string password;
    static std::string multiline;
    static bool        checkbox;

    if(m_ImmediateUserInterface->begin_window(m_ImmediateUserInterface->next_id("Редактируемые текстовые поля", "Input string behaviour testing")))
    {
        m_ImmediateUserInterface->next_content_padding(gs_vec2f(16.f, 16.f));

        if(m_ImmediateUserInterface->begin_vertial_stack(m_ImmediateUserInterface->next_id("VerticalStack")))
        {
            m_ImmediateUserInterface->next_maximum_size(gs_vec2f((float)INT_MAX, 128.f));
            m_ImmediateUserInterface->next_content_padding(gs_vec2f(16.f, 16.f));

            if(m_ImmediateUserInterface->begin_horizontal_stack(m_ImmediateUserInterface->next_id("HorizontalStackRow1")))
            {
                m_ImmediateUserInterface->input_string_singleline(m_ImmediateUserInterface->next_id("UserName"), username);
                m_ImmediateUserInterface->input_string_singleline(
                    m_ImmediateUserInterface->next_id("Password"), password,
                    !checkbox ?
                        ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_Password :
                            ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_Defaults);

                m_ImmediateUserInterface->check_button(
                    m_ImmediateUserInterface->next_id("Checkbox"),
                    ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox,
                    &checkbox);

                m_ImmediateUserInterface->end_horizontal_stack();
            }

            m_ImmediateUserInterface->next_maximum_size(gs_vec2f((float)INT_MAX, 128.f));
            m_ImmediateUserInterface->next_content_padding(gs_vec2f(16.f, 16.f));

            if(m_ImmediateUserInterface->begin_horizontal_stack(m_ImmediateUserInterface->next_id("HorizontalStackRow2")))
            {
                m_ImmediateUserInterface->input_string_singleline(m_ImmediateUserInterface->next_id("UserName"), username);
                m_ImmediateUserInterface->input_string_singleline(
                    m_ImmediateUserInterface->next_id("Password"), password,
                    !checkbox ?
                        ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_Password :
                            ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_Defaults);

                m_ImmediateUserInterface->check_button(
                    m_ImmediateUserInterface->next_id("Checkbox"),
                    ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox,
                    &checkbox);

                m_ImmediateUserInterface->end_horizontal_stack();
            }

            m_ImmediateUserInterface->next_maximum_size(gs_vec2f((float)INT_MAX, (float)INT_MAX));
            m_ImmediateUserInterface->input_string_multiline(m_ImmediateUserInterface->next_id("Multiline"), multiline);

            m_ImmediateUserInterface->end_vertical_stack();
        }

        m_ImmediateUserInterface->end_window();
    }

    // buttons test
    static bool pushed;

    if(m_ImmediateUserInterface->begin_window(m_ImmediateUserInterface->next_id("Кнопки", "Buttons testing")))
    {
        m_ImmediateUserInterface->next_content_padding(gs_vec2f(16.f, 16.f));

        if(m_ImmediateUserInterface->begin_vertial_stack(m_ImmediateUserInterface->next_id("VerticalStack")))
        {
            m_ImmediateUserInterface->next_content_padding(gs_vec2f(16.f, 16.f));

            if(m_ImmediateUserInterface->begin_horizontal_stack(m_ImmediateUserInterface->next_id("HorizontalStackRow1")))
            {
                if(m_ImmediateUserInterface->push_button(m_ImmediateUserInterface->next_id("Кнопанька", "Button")))
                    pushed = !pushed;

                m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("Checkbox"), ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox, &pushed);
                m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("RadioButton"), ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_RadioButton, &pushed);
                m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("SliderButton"), ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_SliderButton, &pushed);
                m_ImmediateUserInterface->end_horizontal_stack();
            }

            m_ImmediateUserInterface->next_content_padding(gs_vec2f(16.f, 16.f));

            if(m_ImmediateUserInterface->begin_horizontal_stack(m_ImmediateUserInterface->next_id("HorizontalStackRow2")))
            {
                if(m_ImmediateUserInterface->push_button(m_ImmediateUserInterface->next_id("Кнопанька", "Button")))
                    pushed = !pushed;

                m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("Checkbox"), ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox, &pushed);
                m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("RadioButton"), ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_RadioButton, &pushed);
                m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("SliderButton"), ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_SliderButton, &pushed);
                
                m_ImmediateUserInterface->end_horizontal_stack();
            }

            m_ImmediateUserInterface->next_content_padding(gs_vec2f(16.f, 16.f));

            if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("ScrollArea")))
            {
                if(m_ImmediateUserInterface->push_button(m_ImmediateUserInterface->next_id("Кнопанька", "Button")))
                    pushed = !pushed;

                m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("Checkbox"), ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox, &pushed);
                m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("RadioButton"), ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_RadioButton, &pushed);
                m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("SliderButton"), ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_SliderButton, &pushed);

                m_ImmediateUserInterface->next_line();

                if(m_ImmediateUserInterface->push_button(m_ImmediateUserInterface->next_id("Кнопанька", "Button1")))
                    pushed = !pushed;

                m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("Checkbox1"), ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox, &pushed);
                m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("RadioButton1"), ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_RadioButton, &pushed);
                m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("SliderButton1"), ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_SliderButton, &pushed);


                m_ImmediateUserInterface->end_scrollarea();
            }

            m_ImmediateUserInterface->end_vertical_stack();
        }

        m_ImmediateUserInterface->end_window();
    }    
    */

    // numeric inputs
    static float        number1 = 0.f;
    static double       number2 = 0.f;
    static int          number3 = 0;
    static unsigned int number4 = 0;

    if(m_ImmediateUserInterface->begin_window(m_ImmediateUserInterface->next_id("Числовой ввод/вывод", "Numeric input testing")))
    {
        m_ImmediateUserInterface->next_content_padding(gs_vec2f(16.f, 16.f));

        if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("ScrollArea")))
        {
            m_ImmediateUserInterface->input_scalar(m_ImmediateUserInterface->next_id("Number1"), number1);
            m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->input_scalar(m_ImmediateUserInterface->next_id("Number2"), number2);
            m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->input_scalar(m_ImmediateUserInterface->next_id("Number3"), number3);
            m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->input_scalar(m_ImmediateUserInterface->next_id("Number4"), number4);

            m_ImmediateUserInterface->end_scrollarea();
        }
        
        m_ImmediateUserInterface->end_window();
    }

    // color pickers
    static gs_color color = 1;

    m_ImmediateUserInterface->next_content_padding(gs_vec2f(16.f, 16.f));

    static bool EnableRGB   = true;
    static bool EnableHSV   = true;
    static bool EnableHSL   = true;
    static bool EnableAlpha = true;
    
    if(m_ImmediateUserInterface->begin_window(m_ImmediateUserInterface->next_id("Регуляторы цвета", "RGBA color picker")))
    {
        if(m_ImmediateUserInterface->begin_scrollarea(
            m_ImmediateUserInterface->next_id("Regulators"),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically |
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally))
        {
            m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("EnableRGB"), EnableRGB);
            m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("EnableRGBLabel"), "RGB");
            
            m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("EnableHSV"), EnableHSV);
            m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("EnableHSVLabel"), "HSV");

            m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("EnableHSL"), EnableHSL);
            m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("EnableHSLLabel"), "HSL");

            m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("EnableAlpha"), EnableAlpha);
            m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("EnableAlphaLabel"), "Alpha");

            m_ImmediateUserInterface->end_scrollarea();
        }

        if(m_ImmediateUserInterface->begin_horizontal_stack(m_ImmediateUserInterface->next_id("Pickers")))
        {
            // RGBA
            m_ImmediateUserInterface->color_picker_rgba(
                m_ImmediateUserInterface->next_id("RGBAColorPicker"),
                color,
                (EnableRGB ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB : 0)
                | (EnableHSV ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV : 0)
                | (EnableHSL ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL : 0)
                | (EnableAlpha ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha : 0));

            // HSVA
            m_ImmediateUserInterface->color_picker_hsva(
                m_ImmediateUserInterface->next_id("HSVAColorPicker"),
                color,
                (EnableRGB ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB : 0)
                | (EnableHSV ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV : 0)
                | (EnableHSL ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL : 0)
                | (EnableAlpha ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha : 0));

            // Color
            m_ImmediateUserInterface->image("ColorImage", color);

            m_ImmediateUserInterface->end_horizontal_stack();
        }
        
        m_ImmediateUserInterface->end_window();
    }
}