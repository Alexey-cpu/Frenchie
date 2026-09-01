// Application
#include <FrenchieApplicationPlatformBackend.hpp>

// Core
#include <FrenchieCoreStringUtilities.hpp>
#include <FrenchieCorePlatform.hpp>

using namespace Frenchie::Application;

std::shared_ptr<FrenchieApplicationPlatformApi> ApplicationPlatformBackend::m_Api = nullptr;

gs_vec2f ApplicationPlatformBackend::get_window_cursor_position()
{
    return m_Api->Input.MouseCursor.Position;
}

gs_vec2f ApplicationPlatformBackend::get_window_cursor_dragdelta()
{
    return m_Api->Input.MouseCursor.DragDelta;
}

gs_vec2f ApplicationPlatformBackend::get_window_size()
{
    return m_Api->Input.WindowSize;
}

gs_vec2f ApplicationPlatformBackend::get_window_position()
{
    return m_Api->Input.WindowPosition;
}

gs_vec2f ApplicationPlatformBackend::get_window_framebuffer_size()
{
    return m_Api->Input.FrameBufferSize;
}

gs_vec2f ApplicationPlatformBackend::get_mouse_wheel_scroll_offset()
{
    return m_Api->Input.MouseScrollOffset;
}

int ApplicationPlatformBackend::key_clicks_count(const ApplicationPlatformBackendKey::Key& _Key)
{
    return m_Api->Input.Keys[_Key].Clicks;
}

std::string ApplicationPlatformBackend::input_text()
{
    return m_Api->Input.Character.has_value() ?
        Frenchie::Core::String::convert_utf32_to_utf8(std::u32string(1, m_Api->Input.Character.value())) :
            std::string();
}

bool ApplicationPlatformBackend::is_window_focused()
{
    return m_Api->Input.Window.Focused;
}

bool ApplicationPlatformBackend::is_window_iconified()
{
    return m_Api->Input.Window.Iconified;
}

bool ApplicationPlatformBackend::has_input_text()
{
    return m_Api->Input.Character.has_value();
}

bool ApplicationPlatformBackend::is_mouse_button_down(const ApplicationPlatformBackendMouseButton::Button& _Button)
{
    return m_Api->Input.MouseButtons[_Button].Down;
}

bool ApplicationPlatformBackend::is_mouse_button_hold(const ApplicationPlatformBackendMouseButton::Button& _Button)
{
    return m_Api->Input.MouseButtons[_Button].Hold;
}

bool ApplicationPlatformBackend::is_mouse_button_pressed(const ApplicationPlatformBackendMouseButton::Button& _Button)
{
    return m_Api->Input.MouseButtons[_Button].Pressed;
}

bool ApplicationPlatformBackend::is_mouse_button_released(const ApplicationPlatformBackendMouseButton::Button& _Button)
{
    return m_Api->Input.MouseButtons[_Button].Released;
}

bool ApplicationPlatformBackend::is_mouse_button_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button)
{
    return m_Api->Input.MouseButtons[_Button].Clicked;
}

bool ApplicationPlatformBackend::is_mouse_button_double_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button)
{
    return m_Api->Input.MouseButtons[_Button].DoubleClicked;
}

int ApplicationPlatformBackend::mouse_button_clicks_count(const ApplicationPlatformBackendMouseButton::Button& _Button)
{
    return m_Api->Input.MouseButtons[_Button].Clicks;
}

bool ApplicationPlatformBackend::is_key_down(const ApplicationPlatformBackendKey::Key& _Key)
{
    return m_Api->Input.Keys[_Key].Down;
}

bool ApplicationPlatformBackend::is_key_hold(const ApplicationPlatformBackendKey::Key& _Key)
{
    return m_Api->Input.Keys[_Key].Hold;
}

bool ApplicationPlatformBackend::is_key_pressed(const ApplicationPlatformBackendKey::Key& _Key)
{
    return m_Api->Input.Keys[_Key].Pressed;
}

bool ApplicationPlatformBackend::is_key_released(const ApplicationPlatformBackendKey::Key& _Key)
{
    return m_Api->Input.Keys[_Key].Released;
}

bool ApplicationPlatformBackend::is_key_clicked(const ApplicationPlatformBackendKey::Key& _Key)
{
    return m_Api->Input.Keys[_Key].Clicked;
}

bool ApplicationPlatformBackend::has_modifier(const ApplicationPlatformBackendKeyModifier::Modifier& _Key)
{
    return m_Api->Input.Modifiers[_Key].Active;
}

void ApplicationPlatformBackend::collect_input()
{
    // update application input input

    // TODO: this MUST BE SETTINGS
    const double KeyClicksCountResetTime = 200; // ms
    const double KeyHoldDetectionTime    = 100; // ms
    const double KeyClickDetectionTime   = 500; // ms

    // handle mouse buttons events
    for (int mouseButton = ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonBegin;
             mouseButton < ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonEnd;
             mouseButton++)
    {
        if(Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].ReleaseTime,
            Frenchie::Core::Clock::tic()) > KeyClicksCountResetTime)
        {
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Clicks = 0;
        }

        if(ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Pressed)
        {
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Down      = true;
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].PressTime = Frenchie::Core::Clock::tic();

            ApplicationPlatformBackend::platform_api()->Input.MouseCursor.MousePressPosition = ApplicationPlatformBackend::platform_api()->Input.MouseCursor.Position;
        }
        
        if(ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Down)
        {
            ApplicationPlatformBackend::platform_api()->Input.MouseCursor.DragDelta =
                ApplicationPlatformBackend::platform_api()->Input.MouseCursor.Position - ApplicationPlatformBackend::platform_api()->Input.MouseCursor.MousePressPosition;

            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Hold =
                Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(
                    ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].PressTime,
                    Frenchie::Core::Clock::tic()) > KeyHoldDetectionTime; // TODO: MUST BE A SETTING
        }

        if(ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Released)
        {
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Down        = false;
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Hold        = false;
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].ReleaseTime = Frenchie::Core::Clock::tic();

            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Clicked =
                Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(
                    ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].PressTime,
                    ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].ReleaseTime) < KeyClickDetectionTime;

            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].DoubleClicked =
                ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Clicked && ++ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Clicks >= 2;
        }        
    }

    // handle keys events
    for (int mouseButton = ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_NamedKey_BEGIN;
             mouseButton < ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_NamedKey_END;
             mouseButton++)
    {
        if(Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].ReleaseTime,
            Frenchie::Core::Clock::tic()) > KeyClicksCountResetTime)
        {
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Clicks = 0;
        }

        if(ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Pressed)
        {
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Down      = true;
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].PressTime = Frenchie::Core::Clock::tic();
        }
        
        if(ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Down)
        {
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Hold =
                Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(
                    ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].PressTime,
                    Frenchie::Core::Clock::tic()) > KeyHoldDetectionTime; // TODO: MUST BE A SETTING
        }

        if(ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Released)
        {
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Down        = false;
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Hold        = false;
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].ReleaseTime = Frenchie::Core::Clock::tic();

            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Clicked =
                Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(
                    ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].PressTime,
                    ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].ReleaseTime) < KeyClickDetectionTime;

            ++ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Clicks;
        }
    }

    // handle key modifiers

    // Ctrl (Command on MacOS)
#ifdef FRENCHIE_APPLICATION_PLATFORM_IS_MACOS
    ApplicationPlatformBackend::platform_api()->Input.Modifiers[ApplicationPlatformBackendKeyModifier::Modifier::ApplicationPlatformBackendKeyModifier_Ctrl].Active =
        ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftSuper].Down ||
            ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightSuper].Down;
#else
    ApplicationPlatformBackend::platform_api()->Input.Modifiers[ApplicationPlatformBackendKeyModifier::Modifier::ApplicationPlatformBackendKeyModifier_Ctrl].Active =
        ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftCtrl].Down ||
            ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightCtrl].Down;
#endif

    // Alt (Option on MacOS)
    ApplicationPlatformBackend::platform_api()->Input.Modifiers[ApplicationPlatformBackendKeyModifier::Modifier::ApplicationPlatformBackendKeyModifier_Alt].Active =
        ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftAlt].Down ||
            ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightAlt].Down;

    // Shift
    ApplicationPlatformBackend::platform_api()->Input.Modifiers[ApplicationPlatformBackendKeyModifier::Modifier::ApplicationPlatformBackendKeyModifier_Shift].Active =
        ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftShift].Down ||
            ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightShift].Down;

    // character input

    // catch character '\n'
    if(ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_Enter].Pressed)
        ApplicationPlatformBackend::platform_api()->Input.Character = '\n';

    // catch character '\t'
    if(ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_Tab].Pressed)
        ApplicationPlatformBackend::platform_api()->Input.Character = '\t';
}

void ApplicationPlatformBackend::restore_input()
{
    // restore mouse buttons
    for (int mouseButton = ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonBegin;
             mouseButton < ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd;
             mouseButton++)
    {
        ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Released      = false;
        ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Pressed       = false;
        ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Clicked       = false;
        ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].DoubleClicked = false;
    }

    // restore keys
    for (int key = ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_BEGIN;
             key < ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_END;
             key++)
    {
        ApplicationPlatformBackend::platform_api()->Input.Keys[key].Released = false;
        ApplicationPlatformBackend::platform_api()->Input.Keys[key].Pressed  = false;
        ApplicationPlatformBackend::platform_api()->Input.Keys[key].Clicked  = false;
    }

    // restore key modifiers
    for (int key = ApplicationPlatformBackendKeyModifier::ApplicationPlatformBackendKeyModifier_Begin;
             key < ApplicationPlatformBackendKeyModifier::ApplicationPlatformBackendKeyModifier_End;
             key++)
    {
        ApplicationPlatformBackend::platform_api()->Input.Modifiers[key].Active = false;
    }

    // restore scroll position
    ApplicationPlatformBackend::platform_api()->Input.MouseScrollOffset = gs_vec2f(0.f, 0.f);

    // restore cursor
    ApplicationPlatformBackend::platform_api()->Input.MouseCursor.DragDelta = gs_vec2f(0.f, 0.f);

    // restore input character
    ApplicationPlatformBackend::platform_api()->Input.Character.reset();
}