#include <FrenchieApplicationPlatformBackend.hpp>

using namespace Frenchie::Application;

ApplicationInput ApplicationPlatformBackend::m_Input;
void* ApplicationPlatformBackend::m_Context = nullptr;

gs_vec2f ApplicationPlatformBackend::get_window_cursor_position()
{
    return m_Input.MouseCursor.Position;
}

gs_vec2f ApplicationPlatformBackend::get_window_cursor_dragdelta()
{
    return m_Input.MouseCursor.DragDelta;
}
bool ApplicationPlatformBackend::is_mouse_button_down(const ApplicationMouseButton::Button& _Button)
{
    return m_Input.MouseButtons[_Button].Down;
}

bool ApplicationPlatformBackend::is_mouse_button_hold(const ApplicationMouseButton::Button& _Button)
{
    return m_Input.MouseButtons[_Button].Hold;
}

bool ApplicationPlatformBackend::is_mouse_button_pressed(const ApplicationMouseButton::Button& _Button)
{
    return m_Input.MouseButtons[_Button].Pressed;
}

bool ApplicationPlatformBackend::is_mouse_button_released(const ApplicationMouseButton::Button& _Button)
{
    return m_Input.MouseButtons[_Button].Released;
}

bool ApplicationPlatformBackend::is_mouse_button_clicked(const ApplicationMouseButton::Button& _Button)
{
    return m_Input.MouseButtons[_Button].Clicked;
}

bool ApplicationPlatformBackend::is_mouse_button_double_clicked(const ApplicationMouseButton::Button& _Button)
{
    return m_Input.MouseButtons[_Button].DoubleClicked;
}