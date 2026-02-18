// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationPlatformBackend.hpp>

// Core
#include <FrenchieCoreSingleton.hpp>

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

    // update application input input

    // mouse buttons
    for (int mouseButton = ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonBegin;
             mouseButton < ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonEnd;
             mouseButton++)
    {
        if(Frenchie::Core::elapsed<std::chrono::milliseconds>(
            ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].ReleaseTime,
            std::chrono::high_resolution_clock::now()) > 200)
        {
            ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].Clicks = 0;
        }

        if(ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].Pressed)
        {
            ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].Down      = true;
            ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].PressTime = Frenchie::Core::tic();

            ApplicationPlatformBackend::m_Input.MouseCursor.MousePressPosition = ApplicationPlatformBackend::m_Input.MouseCursor.Position;
        }
        
        if(ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].Down)
        {
            ApplicationPlatformBackend::m_Input.MouseCursor.DragDelta =
                ApplicationPlatformBackend::m_Input.MouseCursor.Position - ApplicationPlatformBackend::m_Input.MouseCursor.MousePressPosition;

            ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].Hold =
                Frenchie::Core::elapsed<std::chrono::milliseconds>(
                    ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].PressTime,
                    Frenchie::Core::tic()) > 100; // TODO: MUST BE A SETTING
        }

        if(ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].Released)
        {
            ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].Down        = false;
            ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].Hold        = false;
            ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].ReleaseTime = Frenchie::Core::tic();

            ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].Clicked =
                Frenchie::Core::elapsed<std::chrono::milliseconds>(
                    ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].ReleaseTime,
                    ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].PressTime) < 500;

            ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].DoubleClicked =
                ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].Clicked && ++ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].Clicks >= 2;
        }        
    }

    // keys

    // character input
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

void ApplicationInstance::ApplicationInstance::frame_debug()
{
    for(auto layer : m_Layers) 
    {
        if(!layer->is_hidden())
            layer->frame_debug();
    }
}

void ApplicationInstance::ApplicationInstance::frame_finish()
{
    for(auto layer : m_Layers)
    {
        if(!layer->is_hidden())
            layer->frame_finish();
    }

    // restore mouse buttons
    for (int mouseButton = ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonBegin;
             mouseButton < ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd;
             mouseButton++)
    {
        ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].Released      = false;
        ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].Pressed       = false;
        ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].Clicked       = false;
        ApplicationPlatformBackend::m_Input.MouseButtons[mouseButton].DoubleClicked = false;
    }

    // restore cursor
    ApplicationPlatformBackend::m_Input.MouseCursor.DragDelta = gs_vec2f(0.f, 0.f);

    // restore keys
    for (int key = ApplicationPlatformBackendKey::ImGuiKey_NamedKey_BEGIN;
             key < ApplicationPlatformBackendKey::ImGuiKey_End;
             key++)
    {
        ApplicationPlatformBackend::m_Input.Keys[key].Released = false;
        ApplicationPlatformBackend::m_Input.Keys[key].Pressed  = false;
        ApplicationPlatformBackend::m_Input.Keys[key].Clicked  = false;
    }

    // execute backend
    ApplicationPlatformBackend::frame_finish();
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
        return -1;

    while (!is_closed())
    {
        frame_start();
        frame_update();
        frame_debug();
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