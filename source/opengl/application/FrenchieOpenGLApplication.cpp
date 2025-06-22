// Custom
#include <FrenchieOpenGLApplication.hpp>
#include <FrenchieLogger.hpp>

using namespace Frenchie::Application::OpenGL;

Application::Application()
{
    // create and configure rendering context using GLFW
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
        return;
    }

    glfwMakeContextCurrent(m_MainWindow);
}

Application::~Application(){}

bool Application::awake()
{
    // load OpenGL interface using GLAD
    if(m_MainWindow == nullptr || !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        Frenchie::Core::Logger::instance()->error(fmt::format(
            "FRENCHIE::OPENGL::APPLICATION::COULD_NOT_LOAD_GLAD\n"));
        return false;
    }

    // configure viewport
    glViewport(0, 0, 2048, 1024);

    // awake layers
    for(auto it = m_Layers.begin(); it != m_Layers.end(); it++)
    {
        if(!(*it)->awake())
        {
            (*it)->finish();
            auto rm = it;
            it++;
            m_Layers.erase(rm);

            Frenchie::Core::Logger::instance()->error(fmt::format(
                "FRENCHIE::OPENGL::APPLICATION::COULD_NOT_AWAKE_LAYER {}\n", 
                (*it)->get_name()));

            if(it == m_Layers.end())
                break;
        }
    }

    return true;
}

void Application::Application::frame_start()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();

    for(auto layer : m_Layers) 
        layer->frame_start();
}

void Application::Application::frame_update()
{
    for(auto layer : m_Layers) 
        layer->frame_update();
}

void Application::Application::frame_finish()
{
    for(auto layer : m_Layers) 
        layer->frame_finish();

    glfwSwapBuffers(m_MainWindow);
}

void Application::Application::finish()
{
    for(auto layer : m_Layers) 
        layer->finish();
    glfwTerminate();
}

bool Application::Application::is_closed()
{
    return m_Closed || glfwWindowShouldClose(m_MainWindow);
}

void Application::Application::close()
{
    m_Closed = false;
}