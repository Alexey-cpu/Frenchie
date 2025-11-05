#include <FrenchieApplicationPlatformLayer.hpp>

// Application
#include <FrenchieApplication.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        void OpenGLPlatformOnWindowResize(GLFWwindow* _Window, int _Width, int _Height)
        {
            (void)_Window;
            glViewport(0, 0, _Width, _Height);
        }

        void OpenGLPlatformOnWindowMaximizedCallback(GLFWwindow* _Window, int _Maximized)
        {
            int width  = 0;
            int height = 0;
            glfwGetWindowSize(_Window, &width, &height);
            glViewport(0, 0, width, height);
        }
    }
}

Platform::Platform(){}
Platform::~Platform(){}

template<typename Type>
Type* Platform::get_context() const
{
    return reinterpret_cast<Type*>(m_Context);
}

glm::u32vec2 Platform::get_window_size() const
{
    int width  = 0;
    int height = 0;
    glfwGetWindowSize(get_context<GLFWwindow>(), &width, &height);
    return glm::u32vec2(width, height);
}

void Platform::set_window_size(const glm::u32vec2& _Value)
{
    glfwSetWindowSize(get_context<GLFWwindow>(), _Value.x, _Value.y);
}

void Platform::set_maximized(const bool& _Value)
{
    glfwMaximizeWindow(get_context<GLFWwindow>());
}

bool Platform::awake()
{
    // initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // setup Window hints
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);

    if((m_Context = glfwCreateWindow(800, 600, m_Name.c_str(), nullptr, nullptr)) == nullptr)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(get_context<GLFWwindow>());
    glfwSwapInterval(1);

    // setup callbacks
    glfwSetWindowSizeCallback(get_context<GLFWwindow>(), &OpenGLPlatformOnWindowResize);
    glfwSetFramebufferSizeCallback(get_context<GLFWwindow>(), &OpenGLPlatformOnWindowResize);
    glfwSetWindowMaximizeCallback(get_context<GLFWwindow>(), OpenGLPlatformOnWindowMaximizedCallback);

    // load OpenGL interface using GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        m_Context = nullptr;
        return false;
    }

    OpenGLPlatformOnWindowMaximizedCallback(
        get_context<GLFWwindow>(),
        glfwGetWindowAttrib(get_context<GLFWwindow>(), GLFW_MAXIMIZED));

    return true;
}

void Platform::frame_start()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();
}

void Platform::frame_update()
{
    int display_w = 0;
    int display_h = 0;
    glfwGetFramebufferSize(get_context<GLFWwindow>(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    if(glfwWindowShouldClose(get_context<GLFWwindow>()))
        Frenchie::Application::application()->close();
}

void Platform::frame_render()
{
}

void Platform::frame_finish()
{
    glfwSwapBuffers(get_context<GLFWwindow>());
}

void Platform::finish()
{
}

void Platform::quit()
{
    glfwDestroyWindow(get_context<GLFWwindow>());
    glfwTerminate();
}

bool Platform::allows_multiple_instances() const
{
    return false;
}