// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationPlatformBackend.hpp>

// Core
#include <FrenchieCoreSingleton.hpp>

#include <iostream>

using namespace Frenchie::Application;

bool Application::awake()
{
    return ApplicationPlatformBackend::awake();
}

void Application::Application::frame_start()
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
        layer->frame_start();
}

void Application::Application::frame_update()
{
    // execute backend
    ApplicationPlatformBackend::frame_update();

    // execute layers
    for(auto layer : m_Layers)
        layer->frame_update();
}

void Application::Application::frame_render()
{
    for(auto layer : m_Layers)
        layer->frame_render();
}

void Application::Application::frame_input()
{
    for(auto layer : m_Layers)
        layer->frame_input();
}

void Application::Application::frame_finish()
{
    // execute layers
    for(auto layer : m_Layers)
        layer->frame_finish();

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

void Application::Application::finish()
{
    for(auto layer : m_Layers)
        layer->finish();
}

void Application::Application::quit()
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

bool Application::is_closed()
{
    return ApplicationPlatformBackend::is_closed();
}

void Application::close()
{
    ApplicationPlatformBackend::close();
}

int Application::execute()
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

Application::const_iterator Application::begin()
{
    return m_Layers.begin();
}

Application::const_iterator Application::end()
{
    return m_Layers.end();
}

size_t Application::size()
{
    return m_Layers.size();
}

std::list<std::shared_ptr<Layer>> Application::m_Layers = std::list<std::shared_ptr<Layer>>();
std::list<std::shared_ptr<Layer>> Application::m_Awakes = std::list<std::shared_ptr<Layer>>();