// Core
#include <FrenchieCoreStringUtilities.hpp>

// Application
#include <FrenchieApplicationPlatformBackendGLFW.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>

// WINAPI
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windows.h>

using namespace Frenchie::Application;

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

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
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

    // load rendering backend
    if(!ApplicationRenderingBackend::awake(glfwGetWin32Window(reinterpret_cast<GLFWwindow*>(m_Api->Window))))
    {
        glfwTerminate();
        return false;
    }

    // maximize context window
    glfwMaximizeWindow(reinterpret_cast<GLFWwindow*>(m_Api->Window));

    return true;
}