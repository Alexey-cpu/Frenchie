// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationPlatformBackend.hpp>

using namespace Frenchie::Application;

bool App::awake()
{
    return ApplicationPlatformBackend::awake();
}

void App::App::frame_start()
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

void App::App::frame_update()
{
    // execute backend
    ApplicationPlatformBackend::frame_update();

    // execute layers
    for(auto layer : m_Layers)
        layer->frame_update();
}

void App::App::frame_render()
{
    for(auto layer : m_Layers)
        layer->frame_render();
}

void App::App::frame_input()
{
    for(auto layer : m_Layers)
        layer->frame_input();
}

void App::App::frame_finish()
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

void App::App::finish()
{
    for(auto layer : m_Layers)
        layer->finish();
}

void App::App::quit()
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

bool App::is_closed()
{
    return ApplicationPlatformBackend::is_closed();
}

void App::close()
{
    ApplicationPlatformBackend::close();
}

int App::execute()
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

App::const_iterator App::begin()
{
    return m_Layers.begin();
}

App::const_iterator App::end()
{
    return m_Layers.end();
}

std::list<std::shared_ptr<Layer>> App::m_Layers = std::list<std::shared_ptr<Layer>>();
std::list<std::shared_ptr<Layer>> App::m_Awakes = std::list<std::shared_ptr<Layer>>();