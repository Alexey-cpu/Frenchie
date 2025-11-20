#include <FrenchieApplication.hpp>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

using namespace Frenchie::Application;

// Callbacks
void OpenGLPlatformBackendOnWindowResize(GLFWwindow* _Window, int _Width, int _Height)
{
    (void)_Window;
    glViewport(0, 0, _Width, _Height);
}

void OpenGLPlatformBackendOnWindowMaximizedCallback(GLFWwindow* _Window, int _Maximized)
{
    int width  = 0;
    int height = 0;
    glfwGetWindowSize(_Window, &width, &height);
    glViewport(0, 0, width, height);
}


ApplicationInstance::ApplicationInstance()
{
}

ApplicationInstance::~ApplicationInstance()
{
}

bool ApplicationInstance::awake()
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

    // configure context
    glfwMakeContextCurrent(reinterpret_cast<GLFWwindow*>(m_Context));
    glfwSetWindowSizeCallback(reinterpret_cast<GLFWwindow*>(m_Context), &OpenGLPlatformBackendOnWindowResize);
    glfwSetFramebufferSizeCallback(reinterpret_cast<GLFWwindow*>(m_Context), &OpenGLPlatformBackendOnWindowResize);
    glfwSetWindowMaximizeCallback(reinterpret_cast<GLFWwindow*>(m_Context), &OpenGLPlatformBackendOnWindowMaximizedCallback);

    // load OpenGL interface using GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return false;
    }

    // maximize context window
    glfwMaximizeWindow(reinterpret_cast<GLFWwindow*>(m_Context));

    // call window maximize callback if the Window has been maximized
    OpenGLPlatformBackendOnWindowMaximizedCallback(
        reinterpret_cast<GLFWwindow*>(m_Context),
        glfwGetWindowAttrib(reinterpret_cast<GLFWwindow*>(m_Context), GLFW_MAXIMIZED));

    // awake layers
    for(auto it = m_Layers.begin(); it != m_Layers.end(); it++)
    {
        if(!(*it)->awake())
        {
            Frenchie::Core::Logger::instance()->error(fmt::format("Could not awake layer {}", (*it)->get_name()));

            (*it)->finish();
            auto rm = it;
            it++;
            m_Layers.erase(rm);

            if(it == m_Layers.end())
                break;
        }
    }

    return true;
}

void ApplicationInstance::ApplicationInstance::frame_start()
{
    // execute backend
    glfwPollEvents();
    glfwSwapInterval(1);
    // glClear(GL_COLOR_BUFFER_BIT);
    // glClear(GL_DEPTH_BUFFER_BIT);
    // glClear(GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // this can be a rendering command !!!
    glClearColor(128.f / 255.f, 128.f / 255.f, 128.f / 255.f, 255.f / 255.f);

    // remove layers that are closed
    for(auto it = m_Layers.begin(); it != m_Layers.end(); it++)
    {
        if((*it)->is_closed())
        {
            (*it)->finish();
            auto rm = it;
            it++;
            m_Layers.erase(rm);

            if(it == m_Layers.end())
                break;
        }
    }

    // execute layers
    for(auto layer : m_Layers) 
    {
        if(!layer->is_hidden()) 
            layer->frame_start();
    }
}

void ApplicationInstance::ApplicationInstance::frame_update()
{
    // execute backend
    int display_w = 0;
    int display_h = 0;
    glfwGetFramebufferSize(reinterpret_cast<GLFWwindow*>(m_Context), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    // execute layers
    for(auto layer : m_Layers) 
    {
        if(!layer->is_hidden())
            layer->frame_update();
    }
}

void ApplicationInstance::ApplicationInstance::frame_render()
{
    for(auto layer : m_Layers) 
    {
        if(!layer->is_hidden())
            layer->frame_render();
    }
}

void ApplicationInstance::ApplicationInstance::frame_finish()
{
    // swap buffers
    glfwSwapBuffers(reinterpret_cast<GLFWwindow*>(m_Context));

    for(auto layer : m_Layers)
    {
        if(!layer->is_hidden())
            layer->frame_finish();
    }
}

void ApplicationInstance::ApplicationInstance::finish()
{
    for(auto layer : m_Layers)
        layer->finish();
}

void ApplicationInstance::ApplicationInstance::quit()
{
    // deinitialize all application layers
    for(auto layer : m_Layers) 
    {
        layer->close();
        layer->quit();
    }

    // remove all application layers
    m_Layers.clear();

    // destroy context
    glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(m_Context));
    glfwTerminate();
    m_Context = nullptr;
}

std::string ApplicationInstance::get_name() const
{
    return std::string(glfwGetWindowTitle(reinterpret_cast<GLFWwindow*>(m_Context)));
}

gs_vec2f  ApplicationInstance::get_size() const
{
    int x = 0;
    int y = 0;
    glfwGetWindowSize(reinterpret_cast<GLFWwindow*>(m_Context), &x, &y);
    return {x, y};
}

void ApplicationInstance::set_name(const std::string& _Name)
{
    glfwSetWindowTitle(reinterpret_cast<GLFWwindow*>(m_Context), _Name.c_str());
}

bool ApplicationInstance::is_closed()
{
    return glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_Context));
}

void ApplicationInstance::close()
{
    glfwSetWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_Context), GL_TRUE);
}

int ApplicationInstance::execute()
{
    if(!awake()) 
        return -1;

    while (!is_closed())
    {
        frame_start();
        frame_update();
        frame_render();
        frame_finish();
    }

    finish();
    quit();

    return 1;
}

ApplicationInstance::const_iterator ApplicationInstance::begin() const
{
    return m_Layers.begin();
}

ApplicationInstance::const_iterator ApplicationInstance::end() const
{
    return m_Layers.end();
}

size_t ApplicationInstance::size() const
{
    return m_Layers.size();
}

Frenchie::Application::ApplicationInstance* Frenchie::Application::application()
{
    return Frenchie::Core::Singleton<Frenchie::Application::ApplicationInstance>::instance();
}

std::shared_ptr<CommandQueue> Frenchie::Application::application_command_queue()
{
    auto layer = Frenchie::Application::application()->find_layer<CommandQueue>();
    
    if(layer == nullptr) 
        layer = Frenchie::Application::application()->push_layer<CommandQueue>();

    return layer;
}

std::shared_ptr<RenderingQueue> Frenchie::Application::application_rendering_queue()
{
    auto layer = Frenchie::Application::application()->find_layer<RenderingQueue>();
    
    if(layer == nullptr) 
        layer = Frenchie::Application::application()->push_layer<RenderingQueue>();

    return layer;
}