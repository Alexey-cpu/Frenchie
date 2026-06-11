// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationPlatformBackend.hpp>

// Core
#include <FrenchieCoreSingleton.hpp>

#include <iostream>

using namespace Frenchie::Application;

ApplicationInstance::ApplicationInstance()
{
}

ApplicationInstance::~ApplicationInstance()
{
}

bool ApplicationInstance::awake()
{
    return ApplicationPlatformBackend::awake();
}

void ApplicationInstance::ApplicationInstance::frame_start()
{
    // execute backend
    ApplicationPlatformBackend::frame_start();

    // awake layers
    for(auto it = m_Awakes.begin(); it != m_Awakes.end(); it++)
    {
        if((*it)->awake())
            m_Layers.push_back((*it));
    }
    
    m_Awakes.clear();

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
    ApplicationPlatformBackend::frame_update();

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

void ApplicationInstance::ApplicationInstance::frame_input()
{
    for(auto layer : m_Layers) 
    {
        if(!layer->is_hidden())
            layer->frame_input();
    }
}

void ApplicationInstance::ApplicationInstance::frame_finish()
{
    // execute layers
    for(auto layer : m_Layers)
    {
        if(!layer->is_hidden())
            layer->frame_finish();
    }

    // execute backend
    ApplicationPlatformBackend::frame_finish();

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

    // execute backend
    ApplicationPlatformBackend::quit();
}

bool ApplicationInstance::is_closed()
{
    return ApplicationPlatformBackend::is_closed();
}

void ApplicationInstance::close()
{
    ApplicationPlatformBackend::close();
}

int ApplicationInstance::execute()
{
    if(!awake()) 
        return 1;

    while (!is_closed())
    {
        frame_start();
        frame_update();
        frame_input();
        frame_render();
        frame_finish();
    }

    finish();
    quit();

    return 0;
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
