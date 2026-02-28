#include <FrenchieApplication.hpp>

using namespace Frenchie::Application;

// Application
#include <FrenchieApplicationRenderingBackend.hpp>
#include <FrenchieApplicationPlatformBackend.hpp>

// GLFW
#include <GLFW/glfw3.h>

#include <iostream>

// helpers
namespace Frenchie
{
    namespace Application
    {
        class ApplicationInputHandler
        {
        public:

            static ApplicationPlatformBackendMouseButton::Button glfw_mouse_button_to_application_mouse_button(int _MouseButton)
            {
                switch (_MouseButton)
                {
                case GLFW_MOUSE_BUTTON_LEFT:
                    return ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonLeft;
                case GLFW_MOUSE_BUTTON_RIGHT:
                    return ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonRight;
                case GLFW_MOUSE_BUTTON_MIDDLE:
                    return ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonMiddle;
                }

                return ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonEnd;
            }

            static bool glfw_boolean_to_application_boolean(int _Boolean)
            {
                return _Boolean == GLFW_TRUE;
            }

            static ApplicationPlatformBackendKey::Key glfw_key_to_application_key(int _KeyKode, int = -1)
            {
                switch (_KeyKode)
                {
                    case GLFW_KEY_TAB: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Tab;
                    case GLFW_KEY_LEFT: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow;
                    case GLFW_KEY_RIGHT: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow;
                    case GLFW_KEY_UP: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow;
                    case GLFW_KEY_DOWN: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow;
                    case GLFW_KEY_PAGE_UP: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_PageUp;
                    case GLFW_KEY_PAGE_DOWN: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_PageDown;
                    case GLFW_KEY_HOME: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Home;
                    case GLFW_KEY_END: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_End;
                    case GLFW_KEY_INSERT: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Insert;
                    case GLFW_KEY_DELETE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Delete;
                    case GLFW_KEY_BACKSPACE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Backspace;
                    case GLFW_KEY_SPACE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Space;
                    case GLFW_KEY_ENTER: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Enter;
                    case GLFW_KEY_ESCAPE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Escape;
                    case GLFW_KEY_APOSTROPHE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Apostrophe;
                    case GLFW_KEY_COMMA: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Comma;
                    case GLFW_KEY_MINUS: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Minus;
                    case GLFW_KEY_PERIOD: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Period;
                    case GLFW_KEY_SLASH: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Slash;
                    case GLFW_KEY_SEMICOLON: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Semicolon;
                    case GLFW_KEY_EQUAL: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Equal;
                    case GLFW_KEY_LEFT_BRACKET: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftBracket;
                    case GLFW_KEY_BACKSLASH: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Backslash;
                    case GLFW_KEY_WORLD_1: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Oem102;
                    case GLFW_KEY_WORLD_2: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Oem102;
                    case GLFW_KEY_RIGHT_BRACKET: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightBracket;
                    case GLFW_KEY_GRAVE_ACCENT: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_GraveAccent;
                    case GLFW_KEY_CAPS_LOCK: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_CapsLock;
                    case GLFW_KEY_SCROLL_LOCK: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_ScrollLock;
                    case GLFW_KEY_NUM_LOCK: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NumLock;
                    case GLFW_KEY_PRINT_SCREEN: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_PrintScreen;
                    case GLFW_KEY_PAUSE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Pause;
                    case GLFW_KEY_KP_0: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad0;
                    case GLFW_KEY_KP_1: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad1;
                    case GLFW_KEY_KP_2: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad2;
                    case GLFW_KEY_KP_3: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad3;
                    case GLFW_KEY_KP_4: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad4;
                    case GLFW_KEY_KP_5: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad5;
                    case GLFW_KEY_KP_6: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad6;
                    case GLFW_KEY_KP_7: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad7;
                    case GLFW_KEY_KP_8: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad8;
                    case GLFW_KEY_KP_9: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad9;
                    case GLFW_KEY_KP_DECIMAL: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadDecimal;
                    case GLFW_KEY_KP_DIVIDE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadDivide;
                    case GLFW_KEY_KP_MULTIPLY: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadMultiply;
                    case GLFW_KEY_KP_SUBTRACT: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadSubtract;
                    case GLFW_KEY_KP_ADD: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadAdd;
                    case GLFW_KEY_KP_ENTER: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadEnter;
                    case GLFW_KEY_KP_EQUAL: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadEqual;
                    case GLFW_KEY_LEFT_SHIFT: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftShift;
                    case GLFW_KEY_LEFT_CONTROL: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftCtrl;
                    case GLFW_KEY_LEFT_ALT: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftAlt;
                    case GLFW_KEY_LEFT_SUPER: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftSuper;
                    case GLFW_KEY_RIGHT_SHIFT: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightShift;
                    case GLFW_KEY_RIGHT_CONTROL: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightCtrl;
                    case GLFW_KEY_RIGHT_ALT: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightAlt;
                    case GLFW_KEY_RIGHT_SUPER: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightSuper;
                    case GLFW_KEY_MENU: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Menu;
                    case GLFW_KEY_0: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_0;
                    case GLFW_KEY_1: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_1;
                    case GLFW_KEY_2: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_2;
                    case GLFW_KEY_3: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_3;
                    case GLFW_KEY_4: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_4;
                    case GLFW_KEY_5: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_5;
                    case GLFW_KEY_6: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_6;
                    case GLFW_KEY_7: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_7;
                    case GLFW_KEY_8: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_8;
                    case GLFW_KEY_9: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_9;
                    case GLFW_KEY_A: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_A;
                    case GLFW_KEY_B: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_B;
                    case GLFW_KEY_C: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_C;
                    case GLFW_KEY_D: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_D;
                    case GLFW_KEY_E: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_E;
                    case GLFW_KEY_F: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F;
                    case GLFW_KEY_G: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_G;
                    case GLFW_KEY_H: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_H;
                    case GLFW_KEY_I: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_I;
                    case GLFW_KEY_J: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_J;
                    case GLFW_KEY_K: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_K;
                    case GLFW_KEY_L: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_L;
                    case GLFW_KEY_M: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_M;
                    case GLFW_KEY_N: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_N;
                    case GLFW_KEY_O: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_O;
                    case GLFW_KEY_P: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_P;
                    case GLFW_KEY_Q: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Q;
                    case GLFW_KEY_R: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_R;
                    case GLFW_KEY_S: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_S;
                    case GLFW_KEY_T: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_T;
                    case GLFW_KEY_U: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_U;
                    case GLFW_KEY_V: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_V;
                    case GLFW_KEY_W: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_W;
                    case GLFW_KEY_X: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_X;
                    case GLFW_KEY_Y: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Y;
                    case GLFW_KEY_Z: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Z;
                    case GLFW_KEY_F1: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F1;
                    case GLFW_KEY_F2: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F2;
                    case GLFW_KEY_F3: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F3;
                    case GLFW_KEY_F4: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F4;
                    case GLFW_KEY_F5: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F5;
                    case GLFW_KEY_F6: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F6;
                    case GLFW_KEY_F7: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F7;
                    case GLFW_KEY_F8: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F8;
                    case GLFW_KEY_F9: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F9;
                    case GLFW_KEY_F10: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F10;
                    case GLFW_KEY_F11: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F11;
                    case GLFW_KEY_F12: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F12;
                    case GLFW_KEY_F13: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F13;
                    case GLFW_KEY_F14: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F14;
                    case GLFW_KEY_F15: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F15;
                    case GLFW_KEY_F16: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F16;
                    case GLFW_KEY_F17: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F17;
                    case GLFW_KEY_F18: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F18;
                    case GLFW_KEY_F19: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F19;
                    case GLFW_KEY_F20: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F20;
                    case GLFW_KEY_F21: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F21;
                    case GLFW_KEY_F22: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F22;
                    case GLFW_KEY_F23: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F23;
                    case GLFW_KEY_F24: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F24;
                    default: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_None;
                }
            }

            // window callbacks
            static void glfw_on_window_resize_callback(GLFWwindow* _Window, int _Width, int _Height)
            {
                (void)_Window;
                ApplicationRenderingBackend::set_viewport(gs_vec2f(0, 0), gs_vec2f(_Width, _Height));
            }

            static void glfw_on_window_maximized_callback(GLFWwindow* _Window, int _Maximized)
            {
                (void)_Window;
                int width  = 0;
                int height = 0;
                glfwGetWindowSize(_Window, &width, &height);
                ApplicationRenderingBackend::set_viewport(gs_vec2f(0, 0), gs_vec2f(width, height));
            }

            static void glfw_on_window_focused_callback(GLFWwindow* _Window, int _Focused)
            {
                ApplicationPlatformBackend::m_Input.Window.Focused =
                    glfw_boolean_to_application_boolean(_Focused);;
            }

            // cursor callbacks
            static void glfw_on_cursor_moved_callback(GLFWwindow* _Window, double _X, double _Y)
            {
                (void)_Window;
                if(ApplicationPlatformBackend::m_Input.MouseCursor.Entered)
                    ApplicationPlatformBackend::m_Input.MouseCursor.Position = gs_vec2f(_X, _Y);
            }

            static void glfw_on_cursor_enter_callback(GLFWwindow* _Window, int _Entered)
            {
                (void)_Window;
                ApplicationPlatformBackend::m_Input.MouseCursor.Entered =
                    glfw_boolean_to_application_boolean(_Entered);
            }

            // mouse callbacks
            static void glfw_on_mouse_button_callback(GLFWwindow* _Window, int button, int action, int mods)
            {
                (void)_Window;

                // trigger mouse press event
                ApplicationPlatformBackend::m_Input.MouseButtons[glfw_mouse_button_to_application_mouse_button(button)].Pressed  = action == GLFW_PRESS;

                // trigger mouse release event
                ApplicationPlatformBackend::m_Input.MouseButtons[glfw_mouse_button_to_application_mouse_button(button)].Released = action == GLFW_RELEASE;
            }
        
            // keys callbacks
            static void glfw_on_key_callback(GLFWwindow* _Window, int _Keycode, int _Scancode, int _Action, int _Mods)
            {
                (void)_Window;
                (void)_Mods;

                // trigger mouse press event
                ApplicationPlatformBackend::m_Input.Keys[glfw_key_to_application_key(_Keycode, _Scancode)].Pressed  = _Action == GLFW_PRESS;

                // trigger mouse release event
                ApplicationPlatformBackend::m_Input.Keys[glfw_key_to_application_key(_Keycode, _Scancode)].Released = _Action == GLFW_RELEASE;
            }
        
            // input callbacks
            static void glfw_on_character_input_callback(GLFWwindow* _Window, unsigned int _Character)
            {
                (void)_Window;
                ApplicationPlatformBackend::m_Input.Character = _Character;
            }

            // scroll callbacks
            static void glfw_on_mouse_sroll_offset_changed_callback(GLFWwindow* _Window, double _dX, double _dY)
            {
                (void)_Window;
                ApplicationPlatformBackend::m_Input.MouseScrollOffset = gs_vector_normalize(gs_vec2f(_dX, _dY));
            }
        };
    }
}

std::string ApplicationPlatformBackend::get_window_name()
{
    return std::string(glfwGetWindowTitle(reinterpret_cast<GLFWwindow*>(m_Context)));
}

void ApplicationPlatformBackend::set_window_name(const std::string& _Name)
{
    glfwSetWindowTitle(reinterpret_cast<GLFWwindow*>(m_Context), _Name.c_str());
}

bool ApplicationPlatformBackend::awake()
{
    if(m_Context != nullptr)
        return true;

    // initialization
    if(glfwInit() == GLFW_FALSE)
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_ICONIFIED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);

    // create context
    m_Context = glfwCreateWindow(512, 256, "Application", nullptr, nullptr);

    if(m_Context == nullptr)
    {
        glfwTerminate();
        return false;
    }

    // maximize context window
    glfwMaximizeWindow(reinterpret_cast<GLFWwindow*>(m_Context));

    // configure context
    glfwMakeContextCurrent(reinterpret_cast<GLFWwindow*>(m_Context));
    glfwSetWindowSizeCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_window_resize_callback);
    glfwSetFramebufferSizeCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_window_resize_callback);
    glfwSetWindowMaximizeCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_window_maximized_callback);
    glfwSetWindowFocusCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_window_focused_callback);
    glfwSetCursorEnterCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_cursor_enter_callback);
    glfwSetCursorPosCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_cursor_moved_callback);
    glfwSetMouseButtonCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_mouse_button_callback);
    glfwSetScrollCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_mouse_sroll_offset_changed_callback);
    glfwSetKeyCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_key_callback);
    glfwSetCharCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_character_input_callback);
    // glfwSetWindowCloseCallback(vd->Window, ImGui_ImplGlfw_WindowCloseCallback);
    // glfwSetWindowPosCallback(vd->Window, ImGui_ImplGlfw_WindowPosCallback);
    // glfwSetWindowSizeCallback(vd->Window, ImGui_ImplGlfw_WindowSizeCallback);

    // load rendering backend
    if(!ApplicationRenderingBackend::awake((ApplicationRenderingBackend::Loader)glfwGetProcAddress))
    {
        glfwTerminate();
        return false;
    }

    // call window maximize callback if the Window has been maximized
    ApplicationInputHandler::glfw_on_window_maximized_callback(
        reinterpret_cast<GLFWwindow*>(m_Context),
        glfwGetWindowAttrib(reinterpret_cast<GLFWwindow*>(m_Context), GLFW_MAXIMIZED));

    return true;
}

void ApplicationPlatformBackend::frame_start()
{
    // retrieve frame buffer size
    int x = 0;
    int y = 0;
    glfwGetFramebufferSize(reinterpret_cast<GLFWwindow*>(m_Context), &x, &y);
    m_Input.FrameBufferSize = gs_vec2f(x, y);

    // retrieve window size
    glfwGetWindowSize(reinterpret_cast<GLFWwindow*>(m_Context), &x, &y);
    m_Input.WindowSize = gs_vec2f(x, y);

    // retrieve window position
    glfwGetWindowPos(reinterpret_cast<GLFWwindow*>(m_Context), &x, &y);
    m_Input.WindowPosition = gs_vec2f(x, y);

    // execute backend
    glfwPollEvents();
    glfwSwapInterval(1);
}

void ApplicationPlatformBackend::frame_update()
{
    // adjust viewport
    ApplicationRenderingBackend::set_viewport(gs_vec2f(0, 0), m_Input.FrameBufferSize);
}

void ApplicationPlatformBackend::frame_finish()
{
    glfwSwapBuffers(reinterpret_cast<GLFWwindow*>(m_Context));
}

void ApplicationPlatformBackend::quit()
{
    // terminate self
    glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(m_Context));
    glfwTerminate();
    m_Context = nullptr;

    // terminate rendering API
    ApplicationRenderingBackend::quit();
}

bool ApplicationPlatformBackend::is_closed()
{
    return glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_Context));
}

void ApplicationPlatformBackend::close()
{
    glfwSetWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_Context), GL_TRUE);
}