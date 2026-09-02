// Core
#include <FrenchieCoreStringUtilities.hpp>
#include <FrenchieCorePlatform.hpp>

// Application
#include <FrenchieApplicationPlatformBackendSDL3.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>

// WINAPI
#include <windows.h>

using namespace Frenchie::Application;

bool ApplicationPlatformBackend::awake()
{
    if(m_Api != nullptr && m_Api->Window != nullptr)
        return true;

    // initialization
    if(!SDL_Init(SDL_INIT_VIDEO))
        return false;

    // create platform API
    m_Api = std::make_shared<FrenchieApplicationPlatformSDL3>();

    auto SDL3 = platform_api<FrenchieApplicationPlatformSDL3>();

#ifdef FRENCHIE_APPLICATION_PLATFORM_IS_MACOS
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

    // create context
    SDL3->Window =
        SDL_CreateWindow("Application", 512, 256, SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);

    if(m_Api->Window == nullptr)
    {
        SDL_Quit();
        return false;
    }

    SDL_PropertiesID props = SDL_GetWindowProperties(reinterpret_cast<SDL_Window*>(SDL3->Window));
    HWND hwnd = (HWND)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);

    // load rendering backend
    if(!ApplicationRenderingBackend::awake(hwnd))
    {
        SDL_Quit();
        return false;
    }

    return true;
}