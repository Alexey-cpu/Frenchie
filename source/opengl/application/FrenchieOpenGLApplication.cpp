// Custom
#include <FrenchieOpenGLApplication.hpp>
#include <FrenchieLogger.hpp>

using namespace Frenchie::Application::OpenGL;

// callbacks
void OnWindowResize(GLFWwindow* _Window, int _Width, int _Height)
{
    (void)_Window;
    glViewport(0, 0, _Width, _Height);

    Frenchie::Core::Logger::instance()->info("OnWindowResize");
}

void OnWindowMaximizedCallback(GLFWwindow* _Window, int _Maximized)
{
    int width  = 0;
    int height = 0;
    glfwGetWindowSize(_Window, &width, &height);
    glViewport(0, 0, width, height);

    Frenchie::Core::Logger::instance()->info("OnWindowMaximizedCallback");
}

// Application
Application::Application()
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

    if((m_MainWindow = glfwCreateWindow(800, 600, m_Name.c_str(), nullptr, nullptr)) == nullptr)
    {
        glfwTerminate();
        Frenchie::Core::Logger::instance()->error(fmt::format(
            "FRENCHIE::OPENGL::APPLICATION::COULD_NOT_CREATE_WINDOW\n"));
        return;
    }

    glfwMakeContextCurrent(m_MainWindow);

    // setup callbacks
    glfwSetWindowSizeCallback(m_MainWindow, &OnWindowResize);
    glfwSetFramebufferSizeCallback(m_MainWindow, &OnWindowResize);
    glfwSetWindowMaximizeCallback(m_MainWindow, OnWindowMaximizedCallback);

    // load OpenGL interface using GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        m_MainWindow = nullptr;
        Frenchie::Core::Logger::instance()->error(fmt::format(
            "FRENCHIE::OPENGL::APPLICATION::COULD_NOT_LOAD_GLAD\n"));
    }
}

Application::~Application(){}

glm::u32vec2 Application::get_window_size() const
{
    int width  = 0;
    int height = 0;
    glfwGetWindowSize(m_MainWindow, &width, &height);
    return glm::u32vec2(width, height);
}

void Application::set_window_size(const glm::u32vec2& _Value)
{
    glfwSetWindowSize(m_MainWindow, _Value.x, _Value.y);
}

void Application::set_maximized(const bool& _Value)
{
    glfwMaximizeWindow(m_MainWindow);
}

bool Application::awake()
{
    if(m_MainWindow == nullptr) 
        return false;

    // call window callbacks
    OnWindowMaximizedCallback(m_MainWindow, glfwGetWindowAttrib(m_MainWindow, GLFW_MAXIMIZED));

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