#include <FrenchieApplication.hpp>

using namespace Frenchie::Application;

ApplicationInstance::ApplicationInstance()
{
}

ApplicationInstance::~ApplicationInstance()
{
}

bool ApplicationInstance::awake()
{
    return true;
}

void ApplicationInstance::ApplicationInstance::frame_start()
{
    for(auto layer : m_Layers) 
    {
        if(!layer->is_hidden()) 
            layer->frame_start();
    }
}

void ApplicationInstance::ApplicationInstance::frame_update()
{
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
    for(auto layer : m_Layers) 
    {
        layer->close();
        layer->quit();
    }

    m_Layers.clear();
}

std::string ApplicationInstance::get_name() const
{
    return m_Name;
}

void ApplicationInstance::set_name(const std::string& _Name)
{
    m_Name = _Name;
}

bool ApplicationInstance::is_closed()
{
    return !m_Opened;
}

void ApplicationInstance::close()
{
    m_Opened = false;
}

int ApplicationInstance::execute()
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

Frenchie::Core::Reference<Platform> Frenchie::Application::platform()
{
    auto layer = Frenchie::Application::application()->find_layer<Platform>();

    if(layer == nullptr) 
        layer = Frenchie::Application::application()->push_layer<Platform>();

    return layer;
}

Frenchie::Core::Reference<Renderer> Frenchie::Application::renderer()
{
    auto layer = Frenchie::Application::application()->find_layer<Renderer>();

    if(layer == nullptr) 
        layer = Frenchie::Application::application()->push_layer<Renderer>();

    return layer;
}

Frenchie::Core::Reference<Interface> Frenchie::Application::interface()
{
    auto layer = Frenchie::Application::application()->find_layer<Interface>();

    if(layer == nullptr) 
        layer = Frenchie::Application::application()->push_layer<Interface>();

    return layer;
}