#pragma once

#include <FrenchieApplication.hpp>

namespace Frenchie
{
    namespace Application
    {
        class ApplicationPlatformBackend final // TODO: may be implement strategy pattern here ????
        {
        public:
            // API
            static bool awake();
            static void frame_start();
            static void frame_update();
            static void frame_finish();

            static void quit();
            static bool is_closed();
            static void close();

            // getters
            static std::string get_window_name();
            static gs_vec2f    get_window_size();
            static gs_vec2f    get_window_position();
            static gs_vec2f    get_window_cursor_position();
            static gs_vec2f    get_window_cursor_dragdelta();
            static gs_vec2f    get_window_framebuffer_size();

            static bool is_mouse_button_down(const ApplicationMouseButton::Button&);
            static bool is_mouse_button_hold(const ApplicationMouseButton::Button&);
            static bool is_mouse_button_pressed(const ApplicationMouseButton::Button&);
            static bool is_mouse_button_released(const ApplicationMouseButton::Button&);
            static bool is_mouse_button_clicked(const ApplicationMouseButton::Button&);
            static bool is_mouse_button_double_clicked(const ApplicationMouseButton::Button&);

            // setters
            static void ApplicationPlatformBackend::set_window_name(const std::string& _Name);

        //protected:
            static ApplicationInput m_Input;

        private:
            static void*            m_Context;
        };
    }
}