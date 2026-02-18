#include <FrenchieApplication.hpp>

using namespace Frenchie::Application;

// Application
#include <FrenchieApplicationRenderingBackend.hpp>
#include <FrenchieApplicationPlatformBackend.hpp>

// GLFW
#include <GLFW/glfw3.h>

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
                    case GLFW_KEY_TAB: return ApplicationPlatformBackendKey::ImGuiKey_Tab;
                    case GLFW_KEY_LEFT: return ApplicationPlatformBackendKey::ImGuiKey_LeftArrow;
                    case GLFW_KEY_RIGHT: return ApplicationPlatformBackendKey::ImGuiKey_RightArrow;
                    case GLFW_KEY_UP: return ApplicationPlatformBackendKey::ImGuiKey_UpArrow;
                    case GLFW_KEY_DOWN: return ApplicationPlatformBackendKey::ImGuiKey_DownArrow;
                    case GLFW_KEY_PAGE_UP: return ApplicationPlatformBackendKey::ImGuiKey_PageUp;
                    case GLFW_KEY_PAGE_DOWN: return ApplicationPlatformBackendKey::ImGuiKey_PageDown;
                    case GLFW_KEY_HOME: return ApplicationPlatformBackendKey::ImGuiKey_Home;
                    case GLFW_KEY_END: return ApplicationPlatformBackendKey::ImGuiKey_End;
                    case GLFW_KEY_INSERT: return ApplicationPlatformBackendKey::ImGuiKey_Insert;
                    case GLFW_KEY_DELETE: return ApplicationPlatformBackendKey::ImGuiKey_Delete;
                    case GLFW_KEY_BACKSPACE: return ApplicationPlatformBackendKey::ImGuiKey_Backspace;
                    case GLFW_KEY_SPACE: return ApplicationPlatformBackendKey::ImGuiKey_Space;
                    case GLFW_KEY_ENTER: return ApplicationPlatformBackendKey::ImGuiKey_Enter;
                    case GLFW_KEY_ESCAPE: return ApplicationPlatformBackendKey::ImGuiKey_Escape;
                    case GLFW_KEY_APOSTROPHE: return ApplicationPlatformBackendKey::ImGuiKey_Apostrophe;
                    case GLFW_KEY_COMMA: return ApplicationPlatformBackendKey::ImGuiKey_Comma;
                    case GLFW_KEY_MINUS: return ApplicationPlatformBackendKey::ImGuiKey_Minus;
                    case GLFW_KEY_PERIOD: return ApplicationPlatformBackendKey::ImGuiKey_Period;
                    case GLFW_KEY_SLASH: return ApplicationPlatformBackendKey::ImGuiKey_Slash;
                    case GLFW_KEY_SEMICOLON: return ApplicationPlatformBackendKey::ImGuiKey_Semicolon;
                    case GLFW_KEY_EQUAL: return ApplicationPlatformBackendKey::ImGuiKey_Equal;
                    case GLFW_KEY_LEFT_BRACKET: return ApplicationPlatformBackendKey::ImGuiKey_LeftBracket;
                    case GLFW_KEY_BACKSLASH: return ApplicationPlatformBackendKey::ImGuiKey_Backslash;
                    case GLFW_KEY_WORLD_1: return ApplicationPlatformBackendKey::ImGuiKey_Oem102;
                    case GLFW_KEY_WORLD_2: return ApplicationPlatformBackendKey::ImGuiKey_Oem102;
                    case GLFW_KEY_RIGHT_BRACKET: return ApplicationPlatformBackendKey::ImGuiKey_RightBracket;
                    case GLFW_KEY_GRAVE_ACCENT: return ApplicationPlatformBackendKey::ImGuiKey_GraveAccent;
                    case GLFW_KEY_CAPS_LOCK: return ApplicationPlatformBackendKey::ImGuiKey_CapsLock;
                    case GLFW_KEY_SCROLL_LOCK: return ApplicationPlatformBackendKey::ImGuiKey_ScrollLock;
                    case GLFW_KEY_NUM_LOCK: return ApplicationPlatformBackendKey::ImGuiKey_NumLock;
                    case GLFW_KEY_PRINT_SCREEN: return ApplicationPlatformBackendKey::ImGuiKey_PrintScreen;
                    case GLFW_KEY_PAUSE: return ApplicationPlatformBackendKey::ImGuiKey_Pause;
                    case GLFW_KEY_KP_0: return ApplicationPlatformBackendKey::ImGuiKey_Keypad0;
                    case GLFW_KEY_KP_1: return ApplicationPlatformBackendKey::ImGuiKey_Keypad1;
                    case GLFW_KEY_KP_2: return ApplicationPlatformBackendKey::ImGuiKey_Keypad2;
                    case GLFW_KEY_KP_3: return ApplicationPlatformBackendKey::ImGuiKey_Keypad3;
                    case GLFW_KEY_KP_4: return ApplicationPlatformBackendKey::ImGuiKey_Keypad4;
                    case GLFW_KEY_KP_5: return ApplicationPlatformBackendKey::ImGuiKey_Keypad5;
                    case GLFW_KEY_KP_6: return ApplicationPlatformBackendKey::ImGuiKey_Keypad6;
                    case GLFW_KEY_KP_7: return ApplicationPlatformBackendKey::ImGuiKey_Keypad7;
                    case GLFW_KEY_KP_8: return ApplicationPlatformBackendKey::ImGuiKey_Keypad8;
                    case GLFW_KEY_KP_9: return ApplicationPlatformBackendKey::ImGuiKey_Keypad9;
                    case GLFW_KEY_KP_DECIMAL: return ApplicationPlatformBackendKey::ImGuiKey_KeypadDecimal;
                    case GLFW_KEY_KP_DIVIDE: return ApplicationPlatformBackendKey::ImGuiKey_KeypadDivide;
                    case GLFW_KEY_KP_MULTIPLY: return ApplicationPlatformBackendKey::ImGuiKey_KeypadMultiply;
                    case GLFW_KEY_KP_SUBTRACT: return ApplicationPlatformBackendKey::ImGuiKey_KeypadSubtract;
                    case GLFW_KEY_KP_ADD: return ApplicationPlatformBackendKey::ImGuiKey_KeypadAdd;
                    case GLFW_KEY_KP_ENTER: return ApplicationPlatformBackendKey::ImGuiKey_KeypadEnter;
                    case GLFW_KEY_KP_EQUAL: return ApplicationPlatformBackendKey::ImGuiKey_KeypadEqual;
                    case GLFW_KEY_LEFT_SHIFT: return ApplicationPlatformBackendKey::ImGuiKey_LeftShift;
                    case GLFW_KEY_LEFT_CONTROL: return ApplicationPlatformBackendKey::ImGuiKey_LeftCtrl;
                    case GLFW_KEY_LEFT_ALT: return ApplicationPlatformBackendKey::ImGuiKey_LeftAlt;
                    case GLFW_KEY_LEFT_SUPER: return ApplicationPlatformBackendKey::ImGuiKey_LeftSuper;
                    case GLFW_KEY_RIGHT_SHIFT: return ApplicationPlatformBackendKey::ImGuiKey_RightShift;
                    case GLFW_KEY_RIGHT_CONTROL: return ApplicationPlatformBackendKey::ImGuiKey_RightCtrl;
                    case GLFW_KEY_RIGHT_ALT: return ApplicationPlatformBackendKey::ImGuiKey_RightAlt;
                    case GLFW_KEY_RIGHT_SUPER: return ApplicationPlatformBackendKey::ImGuiKey_RightSuper;
                    case GLFW_KEY_MENU: return ApplicationPlatformBackendKey::ImGuiKey_Menu;
                    case GLFW_KEY_0: return ApplicationPlatformBackendKey::ImGuiKey_0;
                    case GLFW_KEY_1: return ApplicationPlatformBackendKey::ImGuiKey_1;
                    case GLFW_KEY_2: return ApplicationPlatformBackendKey::ImGuiKey_2;
                    case GLFW_KEY_3: return ApplicationPlatformBackendKey::ImGuiKey_3;
                    case GLFW_KEY_4: return ApplicationPlatformBackendKey::ImGuiKey_4;
                    case GLFW_KEY_5: return ApplicationPlatformBackendKey::ImGuiKey_5;
                    case GLFW_KEY_6: return ApplicationPlatformBackendKey::ImGuiKey_6;
                    case GLFW_KEY_7: return ApplicationPlatformBackendKey::ImGuiKey_7;
                    case GLFW_KEY_8: return ApplicationPlatformBackendKey::ImGuiKey_8;
                    case GLFW_KEY_9: return ApplicationPlatformBackendKey::ImGuiKey_9;
                    case GLFW_KEY_A: return ApplicationPlatformBackendKey::ImGuiKey_A;
                    case GLFW_KEY_B: return ApplicationPlatformBackendKey::ImGuiKey_B;
                    case GLFW_KEY_C: return ApplicationPlatformBackendKey::ImGuiKey_C;
                    case GLFW_KEY_D: return ApplicationPlatformBackendKey::ImGuiKey_D;
                    case GLFW_KEY_E: return ApplicationPlatformBackendKey::ImGuiKey_E;
                    case GLFW_KEY_F: return ApplicationPlatformBackendKey::ImGuiKey_F;
                    case GLFW_KEY_G: return ApplicationPlatformBackendKey::ImGuiKey_G;
                    case GLFW_KEY_H: return ApplicationPlatformBackendKey::ImGuiKey_H;
                    case GLFW_KEY_I: return ApplicationPlatformBackendKey::ImGuiKey_I;
                    case GLFW_KEY_J: return ApplicationPlatformBackendKey::ImGuiKey_J;
                    case GLFW_KEY_K: return ApplicationPlatformBackendKey::ImGuiKey_K;
                    case GLFW_KEY_L: return ApplicationPlatformBackendKey::ImGuiKey_L;
                    case GLFW_KEY_M: return ApplicationPlatformBackendKey::ImGuiKey_M;
                    case GLFW_KEY_N: return ApplicationPlatformBackendKey::ImGuiKey_N;
                    case GLFW_KEY_O: return ApplicationPlatformBackendKey::ImGuiKey_O;
                    case GLFW_KEY_P: return ApplicationPlatformBackendKey::ImGuiKey_P;
                    case GLFW_KEY_Q: return ApplicationPlatformBackendKey::ImGuiKey_Q;
                    case GLFW_KEY_R: return ApplicationPlatformBackendKey::ImGuiKey_R;
                    case GLFW_KEY_S: return ApplicationPlatformBackendKey::ImGuiKey_S;
                    case GLFW_KEY_T: return ApplicationPlatformBackendKey::ImGuiKey_T;
                    case GLFW_KEY_U: return ApplicationPlatformBackendKey::ImGuiKey_U;
                    case GLFW_KEY_V: return ApplicationPlatformBackendKey::ImGuiKey_V;
                    case GLFW_KEY_W: return ApplicationPlatformBackendKey::ImGuiKey_W;
                    case GLFW_KEY_X: return ApplicationPlatformBackendKey::ImGuiKey_X;
                    case GLFW_KEY_Y: return ApplicationPlatformBackendKey::ImGuiKey_Y;
                    case GLFW_KEY_Z: return ApplicationPlatformBackendKey::ImGuiKey_Z;
                    case GLFW_KEY_F1: return ApplicationPlatformBackendKey::ImGuiKey_F1;
                    case GLFW_KEY_F2: return ApplicationPlatformBackendKey::ImGuiKey_F2;
                    case GLFW_KEY_F3: return ApplicationPlatformBackendKey::ImGuiKey_F3;
                    case GLFW_KEY_F4: return ApplicationPlatformBackendKey::ImGuiKey_F4;
                    case GLFW_KEY_F5: return ApplicationPlatformBackendKey::ImGuiKey_F5;
                    case GLFW_KEY_F6: return ApplicationPlatformBackendKey::ImGuiKey_F6;
                    case GLFW_KEY_F7: return ApplicationPlatformBackendKey::ImGuiKey_F7;
                    case GLFW_KEY_F8: return ApplicationPlatformBackendKey::ImGuiKey_F8;
                    case GLFW_KEY_F9: return ApplicationPlatformBackendKey::ImGuiKey_F9;
                    case GLFW_KEY_F10: return ApplicationPlatformBackendKey::ImGuiKey_F10;
                    case GLFW_KEY_F11: return ApplicationPlatformBackendKey::ImGuiKey_F11;
                    case GLFW_KEY_F12: return ApplicationPlatformBackendKey::ImGuiKey_F12;
                    case GLFW_KEY_F13: return ApplicationPlatformBackendKey::ImGuiKey_F13;
                    case GLFW_KEY_F14: return ApplicationPlatformBackendKey::ImGuiKey_F14;
                    case GLFW_KEY_F15: return ApplicationPlatformBackendKey::ImGuiKey_F15;
                    case GLFW_KEY_F16: return ApplicationPlatformBackendKey::ImGuiKey_F16;
                    case GLFW_KEY_F17: return ApplicationPlatformBackendKey::ImGuiKey_F17;
                    case GLFW_KEY_F18: return ApplicationPlatformBackendKey::ImGuiKey_F18;
                    case GLFW_KEY_F19: return ApplicationPlatformBackendKey::ImGuiKey_F19;
                    case GLFW_KEY_F20: return ApplicationPlatformBackendKey::ImGuiKey_F20;
                    case GLFW_KEY_F21: return ApplicationPlatformBackendKey::ImGuiKey_F21;
                    case GLFW_KEY_F22: return ApplicationPlatformBackendKey::ImGuiKey_F22;
                    case GLFW_KEY_F23: return ApplicationPlatformBackendKey::ImGuiKey_F23;
                    case GLFW_KEY_F24: return ApplicationPlatformBackendKey::ImGuiKey_F24;
                    default: return ApplicationPlatformBackendKey::ImGuiKey_None;
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

            static void glfw_oon_window_focused_callback(GLFWwindow* _Window, int _Focused)
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
            static void glfw_on_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
            {
                (void)window;

                // trigger mouse press event
                ApplicationPlatformBackend::m_Input.MouseButtons[glfw_mouse_button_to_application_mouse_button(button)].Pressed  = action == GLFW_PRESS;

                // trigger mouse release event
                ApplicationPlatformBackend::m_Input.MouseButtons[glfw_mouse_button_to_application_mouse_button(button)].Released = action == GLFW_RELEASE;
            }
        
            // keys callbacks
            static void glfw_on_key_callback(GLFWwindow* window, int keycode, int scancode, int action, int mods)
            {
                (void)window;
                (void)mods;

                // trigger mouse press event
                ApplicationPlatformBackend::m_Input.Keys[glfw_key_to_application_key(keycode, scancode)].Pressed  = action == GLFW_PRESS;

                // trigger mouse release event
                ApplicationPlatformBackend::m_Input.Keys[glfw_key_to_application_key(keycode, scancode)].Released = action == GLFW_RELEASE;
            }
        };
    }
}

std::string ApplicationPlatformBackend::get_window_name()
{
    return std::string(glfwGetWindowTitle(reinterpret_cast<GLFWwindow*>(m_Context)));
}

gs_vec2f ApplicationPlatformBackend::get_window_size()
{
    int x = 0;
    int y = 0;
    glfwGetWindowSize(reinterpret_cast<GLFWwindow*>(m_Context), &x, &y);
    return {x, y};
}

gs_vec2f ApplicationPlatformBackend::get_window_position()
{
    // window
    int x = 0;
    int y = 0;
    glfwGetWindowPos(reinterpret_cast<GLFWwindow*>(m_Context), &x, &y);
    return {x, y};
}

gs_vec2f ApplicationPlatformBackend::get_window_framebuffer_size()
{
    // execute backend
    int display_w = 0;
    int display_h = 0;
    glfwGetFramebufferSize(reinterpret_cast<GLFWwindow*>(m_Context), &display_w, &display_h);
    return gs_vec2f(display_w, display_h);
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
    glfwSetWindowFocusCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_oon_window_focused_callback);
    glfwSetCursorEnterCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_cursor_enter_callback);
    glfwSetCursorPosCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_cursor_moved_callback);
    glfwSetMouseButtonCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_mouse_button_callback);
    // glfwSetScrollCallback(vd->Window, ImGui_ImplGlfw_ScrollCallback);
    glfwSetKeyCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_key_callback);
    // glfwSetCharCallback(vd->Window, ImGui_ImplGlfw_CharCallback);
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
    // execute backend
    glfwPollEvents();
    glfwSwapInterval(1);
}

void ApplicationPlatformBackend::frame_update()
{
    // execute backend
    int display_w = 0;
    int display_h = 0;
    glfwGetFramebufferSize(reinterpret_cast<GLFWwindow*>(m_Context), &display_w, &display_h);
    ApplicationRenderingBackend::set_viewport(gs_vec2f(0, 0), gs_vec2f(display_w, display_h));
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