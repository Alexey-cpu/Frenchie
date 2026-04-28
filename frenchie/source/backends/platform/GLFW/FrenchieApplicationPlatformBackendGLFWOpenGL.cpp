#include <FrenchieApplication.hpp>

using namespace Frenchie::Application;

// Application
#include <FrenchieApplicationPlatformBackendGLFW.hpp>


// ApplicationPlatformBackend
bool ApplicationPlatformBackend::awake()
{
    if(m_Api != nullptr && m_Api->Window != nullptr)
        return true;

    // initialization
    if(glfwInit() == GLFW_FALSE)
        return false;

    // create platform API
    m_Api = std::make_shared<FrenchieApplicationPlatformApi>();

#ifdef FRENCHIE_APPLICATION_PLATFORM_IS_MACOS
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_ICONIFIED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);

    // create context
    m_Api->Window = glfwCreateWindow(512, 256, "Application", nullptr, nullptr);

    if(m_Api->Window == nullptr)
    {
        glfwTerminate();
        return false;
    }

    // maximize context window
    glfwMaximizeWindow(reinterpret_cast<GLFWwindow*>(m_Api->Window));

    // configure context
    glfwMakeContextCurrent(reinterpret_cast<GLFWwindow*>(m_Api->Window));
    glfwSetWindowSizeCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_window_resize_callback);
    glfwSetFramebufferSizeCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_window_resize_callback);
    glfwSetWindowMaximizeCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_window_maximized_callback);
    glfwSetWindowFocusCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_window_focused_callback);
    glfwSetCursorEnterCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_cursor_enter_callback);
    glfwSetCursorPosCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_cursor_moved_callback);
    glfwSetMouseButtonCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_mouse_button_callback);
    glfwSetScrollCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_mouse_sroll_offset_changed_callback);
    glfwSetKeyCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_key_callback);
    glfwSetCharCallback(reinterpret_cast<GLFWwindow*>(m_Api->Window), FrenchieApplicationGLFWInputHandler::glfw_on_character_input_callback);
    // glfwSetWindowCloseCallback(vd->Window, ImGui_ImplGlfw_WindowCloseCallback);
    // glfwSetWindowPosCallback(vd->Window, ImGui_ImplGlfw_WindowPosCallback);
    // glfwSetWindowSizeCallback(vd->Window, ImGui_ImplGlfw_WindowSizeCallback);

    // load rendering backend
    if(!ApplicationRenderingBackend::awake((void*(*)(const char*))glfwGetProcAddress))
    {
        glfwTerminate();
        return false;
    }

    // call window maximize callback if the Window has been maximized
    FrenchieApplicationGLFWInputHandler::glfw_on_window_maximized_callback(
        reinterpret_cast<GLFWwindow*>(m_Api->Window),
        glfwGetWindowAttrib(reinterpret_cast<GLFWwindow*>(m_Api->Window), GLFW_MAXIMIZED));

    return true;
}