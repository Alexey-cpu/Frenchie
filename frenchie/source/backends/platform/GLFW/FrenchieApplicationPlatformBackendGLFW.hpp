#pragma once

// Application
#include <FrenchieApplicationRenderingBackend.hpp>
#include <FrenchieApplicationPlatformBackend.hpp>

// GLFW
#include <GLFW/glfw3.h>

using namespace Frenchie::Application;

// helpers
namespace Frenchie
{
    namespace Application
    {
        class FrenchieApplicationGLFWInputHandler
        {
        public:

            static ApplicationPlatformBackendMouseButton::Button glfw_mouse_button_to_application_mouse_button(int _MouseButton);

            static bool glfw_boolean_to_application_boolean(int _Boolean);

            static ApplicationPlatformBackendKey::Key glfw_key_to_application_key(int _KeyKode, int = -1);

            // window callbacks
            static void glfw_on_window_resize_callback(GLFWwindow* _Window, int _Width, int _Height);
            static void glfw_on_window_maximized_callback(GLFWwindow* _Window, int _Maximized);
            static void glfw_on_window_focused_callback(GLFWwindow* _Window, int _Focused);

            // cursor callbacks
            static void glfw_on_cursor_moved_callback(GLFWwindow* _Window, double _X, double _Y);
            static void glfw_on_cursor_enter_callback(GLFWwindow* _Window, int _Entered);

            // mouse callbacks
            static void glfw_on_mouse_button_callback(GLFWwindow* _Window, int button, int action, int mods);
        
            // keys callbacks
            static void glfw_on_key_callback(GLFWwindow* _Window, int _Keycode, int _Scancode, int _Action, int _Mods);
        
            // input callbacks
            static void glfw_on_character_input_callback(GLFWwindow* _Window, unsigned int _Character);

            // scroll callbacks
            static void glfw_on_mouse_sroll_offset_changed_callback(GLFWwindow* _Window, double _dX, double _dY);

            // window iconify callback
            static void window_iconify_callback(GLFWwindow* _Window, int _Iconified);
        };
    }
}