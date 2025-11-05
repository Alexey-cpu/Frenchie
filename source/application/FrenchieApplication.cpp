#include <FrenchieApplication.hpp>
#include <FrenchieCoreLogger.hpp>

using namespace Frenchie::Application;

// callbacks
void OnWindowResize(GLFWwindow* _Window, int _Width, int _Height)
{
    (void)_Window;
    glViewport(0, 0, _Width, _Height);
}

void OnWindowMaximizedCallback(GLFWwindow* _Window, int _Maximized)
{
    int width  = 0;
    int height = 0;
    glfwGetWindowSize(_Window, &width, &height);
    glViewport(0, 0, width, height);
}

Application::Application()
{
}

Application::~Application()
{
}

bool Application::awake()
{
    return true;
}

void Application::Application::frame_start()
{
    for(auto layer : m_Layers) 
    {
        if(!layer->is_hidden()) 
            layer->frame_start();
    }
}

void Application::Application::frame_update()
{
    for(auto layer : m_Layers) 
    {
        if(!layer->is_hidden())
            layer->frame_update();
    }
}

void Application::Application::frame_render()
{
    for(auto layer : m_Layers) 
    {
        if(!layer->is_hidden())
            layer->frame_render();
    }
}

void Application::Application::frame_finish()
{
    for(auto layer : m_Layers)
    {
        if(!layer->is_hidden())
            layer->frame_finish();
    }
}

void Application::Application::finish()
{
    for(auto layer : m_Layers)
        layer->finish();
}

void Application::Application::quit()
{
    for(auto layer : m_Layers) 
    {
        layer->close();
        layer->quit();
    }

    m_Layers.clear();
}

std::string Application::get_name() const
{
    return m_Name;
}

void Application::set_name(const std::string& _Name)
{
    m_Name = _Name;
}

bool Application::is_closed()
{
    return !m_Opened;
}

void Application::close()
{
    m_Opened = false;
}

int Application::execute()
{
    Frenchie::Application::platform();
    Frenchie::Application::renderer();
    Frenchie::Application::interface();

    if(!awake()) 
        return -1;

    while (!is_closed())
    {
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

        frame_start();
        frame_update();
        frame_render();
        frame_finish();
    }

    finish();
    quit();

    return 1;
}

Application::const_iterator Application::begin() const
{
    return m_Layers.begin();
}

Application::const_iterator Application::end() const
{
    return m_Layers.end();
}

size_t Application::size() const
{
    return m_Layers.size();
}