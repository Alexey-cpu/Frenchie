#include <FrenchieOpenGLApplication.hpp>

using namespace Frenchie::Application::OpenGL;

bool Application::awake()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    //glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    //glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);

    if((m_MainWindow = glfwCreateWindow(
        2048, 
        1024, 
        "Frenchie::Application::OpenGL", 
        nullptr, 
        nullptr)) == nullptr)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_MainWindow);

    // initialize GLAD that gives us an interface to Bulldog::OpenGL function pointers
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return false;
    }

    // setup viewport top left and size according to the previously created Window
    glViewport(0, 0, 2048, 1024);

    return Frenchie::Application::Application::awake();
}

void Application::Application::frame_start()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();
    Frenchie::Application::Application::frame_start();
}

void Application::Application::frame_update()
{
    Frenchie::Application::Application::frame_update();
}

void Application::Application::frame_finish()
{
    Frenchie::Application::Application::frame_finish();
    glfwSwapBuffers(m_MainWindow);
}

void Application::Application::finish()
{
    Frenchie::Application::Application::finish();
    glfwTerminate();
}

bool Application::Application::is_closed()
{
    return m_Closed || glfwWindowShouldClose(m_MainWindow);
}