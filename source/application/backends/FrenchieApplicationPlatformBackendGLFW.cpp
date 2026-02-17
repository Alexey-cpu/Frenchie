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

            static ApplicationMouseButton::Button glfw_mouse_button_to_application_mouse_button(int _MouseButton)
            {
                switch (_MouseButton)
                {
                case GLFW_MOUSE_BUTTON_LEFT:
                    return ApplicationMouseButton::Button::ApplicationMouseButton_Left;
                case GLFW_MOUSE_BUTTON_RIGHT:
                    return ApplicationMouseButton::Button::ApplicationMouseButton_Right;
                case GLFW_MOUSE_BUTTON_MIDDLE:
                    return ApplicationMouseButton::Button::ApplicationMouseButton_Middle;
                }

                return ApplicationMouseButton::Button::ApplicationMouseButton_End;
            }

            static bool glfw_boolean_to_application_boolean(int _Boolean)
            {
                return _Boolean == GLFW_TRUE;
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
    // glfwSetKeyCallback(vd->Window, ImGui_ImplGlfw_KeyCallback);
    // glfwSetCharCallback(vd->Window, ImGui_ImplGlfw_CharCallback);
    // glfwSetWindowCloseCallback(vd->Window, ImGui_ImplGlfw_WindowCloseCallback);
    // glfwSetWindowPosCallback(vd->Window, ImGui_ImplGlfw_WindowPosCallback);
    // glfwSetWindowSizeCallback(vd->Window, ImGui_ImplGlfw_WindowSizeCallback);

    // load rendering API
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
    glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(m_Context));
    glfwTerminate();
    m_Context = nullptr;
}

bool ApplicationPlatformBackend::is_closed()
{
    return glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_Context));
}

void ApplicationPlatformBackend::close()
{
    glfwSetWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_Context), GL_TRUE);
}