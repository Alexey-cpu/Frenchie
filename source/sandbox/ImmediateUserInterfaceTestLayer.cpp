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
            Frenchie::Core::String::format("Количество комманд %d", m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().RenderingCommandsCount));

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
                m_ImmediateUserInterface->check_button("Checkbox", ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox, &checked);
                m_ImmediateUserInterface->menu_action("Action-4");

                m_ImmediateUserInterface->check_button("RadioButton", ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_RadioButton, &checked);
                m_ImmediateUserInterface->menu_action("Action-5");

                m_ImmediateUserInterface->check_button("SliderButton", ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_SliderButton, &checked);
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

void ImmediateUserInterfaceTestLayer::scrollarea_test()
{
    m_ImmediateUserInterface->m_Settings =
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_DisableDocking;

    m_ImmediateUserInterface->next_content_padding(gs_vec2f(16.f));

    if(m_ImmediateUserInterface->begin_scrollarea(
        "ScrollArea",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults))
    {
        int k = 0;

        for(int i = 0 ; i < 5; i++)
        {
            for(int j = 0 ; j < 3; j++)
                m_ImmediateUserInterface->push_button(std::string("Button-").append(std::to_string(k++)));

            m_ImmediateUserInterface->next_line();
        }

        m_ImmediateUserInterface->end_scrollarea();
    }
}

void ImmediateUserInterfaceTestLayer::widgets_test()
{
    m_ImmediateUserInterface->m_Settings =
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking |
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes;

    static std::string input1;
    static std::string input2;
    static std::string input3;
    static std::string input4;

    static std::string input5;

    if(m_ImmediateUserInterface->begin_window("Window-2"))
    {
        m_ImmediateUserInterface->input_string_multiline("Window-2/Text", input5);

        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window("Window-1"))
    {
        // m_ImmediateUserInterface->next_minimum_size(gs_vec2f(128.f, 128.f));
        // m_ImmediateUserInterface->next_maximum_size(gs_vec2f(512.f, 512.f));

        if(m_ImmediateUserInterface->begin_vertial_stack("Window-1/VerticalStack"))
        {
            m_ImmediateUserInterface->next_content_padding(gs_vec2f(16.f, 0.f));

            if(m_ImmediateUserInterface->begin_horizontal_stack("Window-1/VerticalStack/Row-1"))
            {
                m_ImmediateUserInterface->label("Window-1/VerticalStack/Row-1/Input-1-Label", "Input-1"); m_ImmediateUserInterface->input_string_singleline("Window-1/VerticalStack/Row-1/Input-1", input1);
                m_ImmediateUserInterface->label("Window-1/VerticalStack/Row-1/Input-2-Label", "Input-2"); m_ImmediateUserInterface->input_string_singleline("Window-1/VerticalStack/Row-1/Input-2", input2);

                m_ImmediateUserInterface->end_horizontal_stack();
            }

            m_ImmediateUserInterface->next_content_padding(gs_vec2f(16.f, 16.f));

            if(m_ImmediateUserInterface->begin_vertial_stack("Window-2/VerticalStack/Row-2"))
            {
                m_ImmediateUserInterface->input_string_singleline("Window-1/VerticalStack/Row-2/Input-3", input3);
                m_ImmediateUserInterface->input_string_singleline("Window-1/VerticalStack/Row-2/Input-4", input4);

                m_ImmediateUserInterface->end_vertical_stack();
            }

            m_ImmediateUserInterface->end_vertical_stack();
        }

        m_ImmediateUserInterface->end_window();
    }
}

void ImmediateUserInterfaceTestLayer::renderer_test()
{
    m_ImmediateUserInterface->m_Settings &=
        ~ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking;

    m_ImmediateUserInterface->m_Renderer->push_rectangle_rounded(
        gs_vec2f(512.f, 512.f),
        gs_vec2f(1024.f, 1024.f),
        64.f,
        32.f,
        gs_rgba_color(255, 0, 0, 255));
}

void ImmediateUserInterfaceTestLayer::platform_backend_test()
{
    m_ImmediateUserInterface->m_Settings =
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking |
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes;

    if(m_ImmediateUserInterface->begin_window("Окно тестирования обертки backend-а ОС###PlatformBackendTestWindow"))
    {
        if(m_ImmediateUserInterface->begin_scrollarea("PlatformBackendTestWindow/ScrollArea"))
        {
            // key events
            static std::string keyDownName     = ApplicationPlatformBackendKey::to_string(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_None);
            static std::string keyHoldName     = ApplicationPlatformBackendKey::to_string(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_None);
            static std::string keyPressedName  = ApplicationPlatformBackendKey::to_string(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_None);
            static std::string keyReleasedName = ApplicationPlatformBackendKey::to_string(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_None);
            static std::string keyClickedName  = ApplicationPlatformBackendKey::to_string(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_None);
            static int         keyClicksCount  = 0;

            for(int key = ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_BEGIN;
                    key < ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_END;
                    key++)
            {
                if(ApplicationPlatformBackend::is_key_down((ApplicationPlatformBackendKey::Key)key))
                    keyDownName = ApplicationPlatformBackendKey::to_string((ApplicationPlatformBackendKey::Key)key);

                if(ApplicationPlatformBackend::is_key_hold((ApplicationPlatformBackendKey::Key)key))
                    keyHoldName = ApplicationPlatformBackendKey::to_string((ApplicationPlatformBackendKey::Key)key);

                if(ApplicationPlatformBackend::is_key_pressed((ApplicationPlatformBackendKey::Key)key))
                    keyPressedName = ApplicationPlatformBackendKey::to_string((ApplicationPlatformBackendKey::Key)key);

                if(ApplicationPlatformBackend::is_key_released((ApplicationPlatformBackendKey::Key)key))
                    keyReleasedName = ApplicationPlatformBackendKey::to_string((ApplicationPlatformBackendKey::Key)key);

                if(ApplicationPlatformBackend::is_key_clicked((ApplicationPlatformBackendKey::Key)key))
                {
                    keyClickedName = ApplicationPlatformBackendKey::to_string((ApplicationPlatformBackendKey::Key)key);
                    keyClicksCount = ApplicationPlatformBackend::key_clicks_count((ApplicationPlatformBackendKey::Key)key);
                }
            }

            m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->label("PlatformBackendTestWindow/ScrollArea/KeyEvents", "KeyEvents:"); m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->label("PlatformBackendTestWindow/ScrollArea/keyDownName", Frenchie::Core::String::format("Down: [%s]", keyDownName.c_str())); m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->label("PlatformBackendTestWindow/ScrollArea/keyHoldName", Frenchie::Core::String::format("Hold: [%s]", keyHoldName.c_str())); m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->label("PlatformBackendTestWindow/ScrollArea/keyPressedName", Frenchie::Core::String::format("Pressed: [%s]", keyPressedName.c_str())); m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->label("PlatformBackendTestWindow/ScrollArea/keyReleasedName", Frenchie::Core::String::format("Released: [%s]", keyReleasedName.c_str())); m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->label("PlatformBackendTestWindow/ScrollArea/keyClickedName", Frenchie::Core::String::format("Clicked: [%s]", keyClickedName.c_str())); m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->label("PlatformBackendTestWindow/ScrollArea/keyClicks", Frenchie::Core::String::format("Clicks: [%d]", keyClicksCount)); m_ImmediateUserInterface->next_line();

            // mouse events
            static std::string mouseButtonDownName          = ApplicationPlatformBackendMouseButton::to_string(ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd);
            static std::string mouseButtonHoldName          = ApplicationPlatformBackendMouseButton::to_string(ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd);
            static std::string mouseButtonPressedName       = ApplicationPlatformBackendMouseButton::to_string(ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd);
            static std::string mouseButtonReleasedName      = ApplicationPlatformBackendMouseButton::to_string(ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd);
            static std::string mouseButtonClickedName       = ApplicationPlatformBackendMouseButton::to_string(ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd);
            static std::string mouseButtonDoubleClickedName = ApplicationPlatformBackendMouseButton::to_string(ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd);
            static int         mouseButtonClicksCount       = 0;

            for(int key = ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonBegin;
                    key < ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd;
                    key++)
            {
                if(ApplicationPlatformBackend::is_mouse_button_down((ApplicationPlatformBackendMouseButton::Button)key))
                    mouseButtonDownName = ApplicationPlatformBackendMouseButton::to_string((ApplicationPlatformBackendMouseButton::Button)key);

                if(ApplicationPlatformBackend::is_mouse_button_hold((ApplicationPlatformBackendMouseButton::Button)key))
                    mouseButtonHoldName = ApplicationPlatformBackendMouseButton::to_string((ApplicationPlatformBackendMouseButton::Button)key);

                if(ApplicationPlatformBackend::is_mouse_button_pressed((ApplicationPlatformBackendMouseButton::Button)key))
                    mouseButtonPressedName = ApplicationPlatformBackendMouseButton::to_string((ApplicationPlatformBackendMouseButton::Button)key);

                if(ApplicationPlatformBackend::is_mouse_button_released((ApplicationPlatformBackendMouseButton::Button)key))
                    mouseButtonReleasedName = ApplicationPlatformBackendMouseButton::to_string((ApplicationPlatformBackendMouseButton::Button)key);

                if(ApplicationPlatformBackend::is_mouse_button_clicked((ApplicationPlatformBackendMouseButton::Button)key))
                {
                    mouseButtonClickedName = ApplicationPlatformBackendMouseButton::to_string((ApplicationPlatformBackendMouseButton::Button)key);
                    mouseButtonClicksCount = ApplicationPlatformBackend::mouse_button_clicks_count((ApplicationPlatformBackendMouseButton::Button)key);
                }

                if(ApplicationPlatformBackend::is_mouse_button_double_clicked((ApplicationPlatformBackendMouseButton::Button)key))
                    mouseButtonDoubleClickedName = ApplicationPlatformBackendMouseButton::to_string((ApplicationPlatformBackendMouseButton::Button)key);
            }

            m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->label("PlatformBackendTestWindow/ScrollArea/MouseEvents", "MouseEvents:"); m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->label("PlatformBackendTestWindow/ScrollArea/MouseDownName", Frenchie::Core::String::format("Down: [%s]", mouseButtonDownName.c_str())); m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->label("PlatformBackendTestWindow/ScrollArea/MouseHoldName", Frenchie::Core::String::format("Hold: [%s]", mouseButtonHoldName.c_str())); m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->label("PlatformBackendTestWindow/ScrollArea/MousePressedName", Frenchie::Core::String::format("Pressed: [%s]", mouseButtonPressedName.c_str())); m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->label("PlatformBackendTestWindow/ScrollArea/MouseReleasedName", Frenchie::Core::String::format("Released: [%s]", mouseButtonReleasedName.c_str())); m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->label("PlatformBackendTestWindow/ScrollArea/MouseClickedName", Frenchie::Core::String::format("Clicked: [%s]", mouseButtonClickedName.c_str())); m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->label("PlatformBackendTestWindow/ScrollArea/MouseDoubleClickedName", Frenchie::Core::String::format("Double clicked: [%s]", mouseButtonDoubleClickedName.c_str())); m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->label("PlatformBackendTestWindow/ScrollArea/MouseClicksCount", Frenchie::Core::String::format("Clicks: [%d]", mouseButtonClicksCount)); m_ImmediateUserInterface->next_line();
            
            // input text test
            static std::string inputText;

            if(!ApplicationPlatformBackend::input_text().empty())
                inputText = ApplicationPlatformBackend::input_text();

            m_ImmediateUserInterface->next_line();
            m_ImmediateUserInterface->label("PlatformBackendTestWindow/ScrollArea/InputText", Frenchie::Core::String::format("Text: [%s]", inputText.c_str()));
            
            m_ImmediateUserInterface->end_scrollarea();
        }

        m_ImmediateUserInterface->end_window();
    }
}