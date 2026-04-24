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

    // TODO: this MUST BE SETTINGS
    const double KeyClicksCountResetTime = 200; // ms
    const double KeyHoldDetectionTime    = 100; // ms
    const double KeyClickDetectionTime   = 500; // ms

    // handle mouse buttons events
    for (int mouseButton = ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonBegin;
             mouseButton < ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonEnd;
             mouseButton++)
    {
        if(Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::HighResolutionClockMilliseconds>(
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].ReleaseTime,
            Frenchie::Core::Clock::tic()) > KeyClicksCountResetTime)
        {
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Clicks = 0;
        }

        if(ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Pressed)
        {
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Down      = true;
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].PressTime = Frenchie::Core::Clock::tic();

            ApplicationPlatformBackend::platform_api()->Input.MouseCursor.MousePressPosition = ApplicationPlatformBackend::platform_api()->Input.MouseCursor.Position;
        }
        
        if(ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Down)
        {
            ApplicationPlatformBackend::platform_api()->Input.MouseCursor.DragDelta =
                ApplicationPlatformBackend::platform_api()->Input.MouseCursor.Position - ApplicationPlatformBackend::platform_api()->Input.MouseCursor.MousePressPosition;

            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Hold =
                Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::HighResolutionClockMilliseconds>(
                    ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].PressTime,
                    Frenchie::Core::Clock::tic()) > KeyHoldDetectionTime; // TODO: MUST BE A SETTING
        }

        if(ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Released)
        {
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Down        = false;
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Hold        = false;
            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].ReleaseTime = Frenchie::Core::Clock::tic();

            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Clicked =
                Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::HighResolutionClockMilliseconds>(
                    ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].PressTime,
                    ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].ReleaseTime) < KeyClickDetectionTime;

            ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].DoubleClicked =
                ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Clicked && ++ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Clicks >= 2;
        }        
    }

    // handle keys events
    for (int mouseButton = ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_NamedKey_BEGIN;
             mouseButton < ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_NamedKey_END;
             mouseButton++)
    {
        if(Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::HighResolutionClockMilliseconds>(
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].ReleaseTime,
            Frenchie::Core::Clock::tic()) > KeyClicksCountResetTime)
        {
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Clicks = 0;
        }

        if(ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Pressed)
        {
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Down      = true;
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].PressTime = Frenchie::Core::Clock::tic();
        }
        
        if(ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Down)
        {
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Hold =
                Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::HighResolutionClockMilliseconds>(
                    ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].PressTime,
                    Frenchie::Core::Clock::tic()) > KeyHoldDetectionTime; // TODO: MUST BE A SETTING
        }

        if(ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Released)
        {
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Down        = false;
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Hold        = false;
            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].ReleaseTime = Frenchie::Core::Clock::tic();

            ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Clicked =
                Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::HighResolutionClockMilliseconds>(
                    ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].PressTime,
                    ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].ReleaseTime) < KeyClickDetectionTime;

            ++ApplicationPlatformBackend::platform_api()->Input.Keys[mouseButton].Clicks;
        }
    }

    // handle key modifiers

    // Ctrl (Command on MacOS)
#ifdef FRENCHIE_APPLICATION_PLATFORM_IS_MACOS
    ApplicationPlatformBackend::platform_api()->Input.Modifiers[ApplicationPlatformBackendKeyModifier::Modifier::ApplicationPlatformBackendKeyModifier_Ctrl].Active =
        ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftSuper].Down ||
            ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightSuper].Down;
#else
    ApplicationPlatformBackend::platform_api()->Input.Modifiers[ApplicationPlatformBackendKeyModifier::Modifier::ApplicationPlatformBackendKeyModifier_Ctrl].Active =
        ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftCtrl].Down ||
            ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightCtrl].Down;
#endif

    // Alt (Option on MacOS)
    ApplicationPlatformBackend::platform_api()->Input.Modifiers[ApplicationPlatformBackendKeyModifier::Modifier::ApplicationPlatformBackendKeyModifier_Alt].Active =
        ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftAlt].Down ||
            ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightAlt].Down;

    // Shift
    ApplicationPlatformBackend::platform_api()->Input.Modifiers[ApplicationPlatformBackendKeyModifier::Modifier::ApplicationPlatformBackendKeyModifier_Shift].Active =
        ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftShift].Down ||
            ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightShift].Down;

    // character input

    // catch character '\n'
    if(ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_Enter].Pressed)
        ApplicationPlatformBackend::platform_api()->Input.Character = '\n';

    // catch character '\t'
    if(ApplicationPlatformBackend::platform_api()->Input.Keys[ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_Tab].Pressed)
        ApplicationPlatformBackend::platform_api()->Input.Character = '\t';
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
        ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Released      = false;
        ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Pressed       = false;
        ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].Clicked       = false;
        ApplicationPlatformBackend::platform_api()->Input.MouseButtons[mouseButton].DoubleClicked = false;
    }

    // restore keys
    for (int key = ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_BEGIN;
             key < ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_END;
             key++)
    {
        ApplicationPlatformBackend::platform_api()->Input.Keys[key].Released = false;
        ApplicationPlatformBackend::platform_api()->Input.Keys[key].Pressed  = false;
        ApplicationPlatformBackend::platform_api()->Input.Keys[key].Clicked  = false;
    }

    // restore key modifiers
    for (int key = ApplicationPlatformBackendKeyModifier::ApplicationPlatformBackendKeyModifier_Begin;
             key < ApplicationPlatformBackendKeyModifier::ApplicationPlatformBackendKeyModifier_End;
             key++)
    {
        ApplicationPlatformBackend::platform_api()->Input.Modifiers[key].Active = false;
    }

    // restore scroll position
    ApplicationPlatformBackend::platform_api()->Input.MouseScrollOffset = gs_vec2f(0.f, 0.f);

    // restore cursor
    ApplicationPlatformBackend::platform_api()->Input.MouseCursor.DragDelta = gs_vec2f(0.f, 0.f);

    // restore input character
    ApplicationPlatformBackend::platform_api()->Input.Character.reset();

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
        frame_input();
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
