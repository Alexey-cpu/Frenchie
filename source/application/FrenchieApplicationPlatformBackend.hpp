#pragma once

// Core
#include <FrenchieCoreMath.hpp>
#include <FrenchieCoreClock.hpp>

// STL
#include <string>

namespace Frenchie
{
    namespace Application
    {
        struct ApplicationPlatformBackendMouseButton
        {
            enum Button : int
            {
                ApplicationPlatformBackendMouseButtonBegin,
                ApplicationPlatformBackendMouseButtonLeft = ApplicationPlatformBackendMouseButtonBegin,
                ApplicationPlatformBackendMouseButtonRight,
                ApplicationPlatformBackendMouseButtonMiddle,
                ApplicationPlatformBackendMouseButtonEnd
            };

            int                                            Clicks       {0    };
            bool                                           Down         {false};
            bool                                           Hold         {false};
            bool                                           Pressed      {false};
            bool                                           Released     {false};
            bool                                           Clicked      {false};
            bool                                           DoubleClicked{false};
            std::chrono::high_resolution_clock::time_point PressTime    {std::chrono::high_resolution_clock::time_point()};
            std::chrono::high_resolution_clock::time_point ReleaseTime  {std::chrono::high_resolution_clock::time_point()};
        };

        struct ApplicationPlatformBackendMouseCursor
        {
            bool      Entered           {false};
            gs_vec2f  Position          {gs_vec2f(0.f)};
            gs_vec2f  MousePressPosition{gs_vec2f(0.f)};
            gs_vec2f  DragDelta         {gs_vec2f(0.f)};
        };

        struct ApplicationPlatformBackendWindow
        {
            bool Focused {false};
        };

        struct ApplicationPlatformBackendKey
        {
            enum Key : int
            {
                ApplicationPlatformBackendNone,
                ApplicationPlatformBackendEnd,
            };

            int                                            Clicks       {0    };
            bool                                           Down         {false};
            bool                                           Hold         {false};
            bool                                           Pressed      {false};
            bool                                           Released     {false};
            bool                                           Clicked      {false};
            std::chrono::high_resolution_clock::time_point PressTime    {std::chrono::high_resolution_clock::time_point()};
            std::chrono::high_resolution_clock::time_point ReleaseTime  {std::chrono::high_resolution_clock::time_point()};
        };

        struct ApplicationPlatformBackendInput
        {            
            ApplicationPlatformBackendMouseButton MouseButtons[ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonEnd]{};
            ApplicationPlatformBackendMouseCursor MouseCursor {ApplicationPlatformBackendMouseCursor()};
            ApplicationPlatformBackendWindow      Window      {ApplicationPlatformBackendWindow()};
            ApplicationPlatformBackendKey         Keys        [ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendEnd]{};
        };

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

            static bool is_mouse_button_down(const ApplicationPlatformBackendMouseButton::Button&);
            static bool is_mouse_button_hold(const ApplicationPlatformBackendMouseButton::Button&);
            static bool is_mouse_button_pressed(const ApplicationPlatformBackendMouseButton::Button&);
            static bool is_mouse_button_released(const ApplicationPlatformBackendMouseButton::Button&);
            static bool is_mouse_button_clicked(const ApplicationPlatformBackendMouseButton::Button&);
            static bool is_mouse_button_double_clicked(const ApplicationPlatformBackendMouseButton::Button&);

            // setters
            static void set_window_name(const std::string& _Name);

            // info
            static ApplicationPlatformBackendInput m_Input;

        private:

            // context
            static void* m_Context;
        };
    }
}