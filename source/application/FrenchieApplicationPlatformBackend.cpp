#include <FrenchieApplicationPlatformBackend.hpp>

// Core
#include <FrenchieCoreStringUtilities.hpp>

using namespace Frenchie::Application;

ApplicationPlatformBackendInput ApplicationPlatformBackend::m_Input;
void* ApplicationPlatformBackend::m_Context = nullptr;

gs_vec2f ApplicationPlatformBackend::get_window_cursor_position()
{
    return m_Input.MouseCursor.Position;
}

gs_vec2f ApplicationPlatformBackend::get_window_cursor_dragdelta()
{
    return m_Input.MouseCursor.DragDelta;
}
bool ApplicationPlatformBackend::is_mouse_button_down(const ApplicationPlatformBackendMouseButton::Button& _Button)
{
    return m_Input.MouseButtons[_Button].Down;
}

bool ApplicationPlatformBackend::is_mouse_button_hold(const ApplicationPlatformBackendMouseButton::Button& _Button)
{
    return m_Input.MouseButtons[_Button].Hold;
}

bool ApplicationPlatformBackend::is_mouse_button_pressed(const ApplicationPlatformBackendMouseButton::Button& _Button)
{
    return m_Input.MouseButtons[_Button].Pressed;
}

bool ApplicationPlatformBackend::is_mouse_button_released(const ApplicationPlatformBackendMouseButton::Button& _Button)
{
    return m_Input.MouseButtons[_Button].Released;
}

bool ApplicationPlatformBackend::is_mouse_button_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button)
{
    return m_Input.MouseButtons[_Button].Clicked;
}

bool ApplicationPlatformBackend::is_mouse_button_double_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button)
{
    return m_Input.MouseButtons[_Button].DoubleClicked;
}

int ApplicationPlatformBackend::mouse_button_clicks_count(const ApplicationPlatformBackendMouseButton::Button& _Button)
{
    return m_Input.MouseButtons[_Button].Clicks;
}

bool ApplicationPlatformBackend::is_key_down(const ApplicationPlatformBackendKey::Key& _Key)
{
    return m_Input.Keys[_Key].Down;
}

bool ApplicationPlatformBackend::is_key_hold(const ApplicationPlatformBackendKey::Key& _Key)
{
    return m_Input.Keys[_Key].Hold;
}

bool ApplicationPlatformBackend::is_key_pressed(const ApplicationPlatformBackendKey::Key& _Key)
{
    return m_Input.Keys[_Key].Pressed;
}

bool ApplicationPlatformBackend::is_key_released(const ApplicationPlatformBackendKey::Key& _Key)
{
    return m_Input.Keys[_Key].Released;
}

bool ApplicationPlatformBackend::is_key_clicked(const ApplicationPlatformBackendKey::Key& _Key)
{
    return m_Input.Keys[_Key].Clicked;
}

int ApplicationPlatformBackend::key_clicks_count(const ApplicationPlatformBackendKey::Key& _Key)
{
    return m_Input.Keys[_Key].Clicks;
}

std::string ApplicationPlatformBackend::input_text()
{
    return m_Input.Character.has_value() ?
        Frenchie::Core::String::convert_utf32_to_utf8(std::u32string(1, m_Input.Character.value())) :
            std::string();
}