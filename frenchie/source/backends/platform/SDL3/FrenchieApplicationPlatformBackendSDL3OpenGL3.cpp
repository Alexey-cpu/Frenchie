#include <FrenchieApplication.hpp>

#include <FrenchieCoreStringUtilities.hpp>

#include <FrenchieApplicationPlatformBackendSDL3.hpp>

using namespace Frenchie::Application;

// Application
#include <FrenchieApplicationRenderingBackend.hpp>

bool ApplicationPlatformBackend::on_awake()
{
    if(m_Api != nullptr && m_Api->Window != nullptr)
        return true;

    // initialization
    if(!SDL_Init(SDL_INIT_VIDEO))
        return false;

    // create platform API
    m_Api = std::make_shared<FrenchieApplicationPlatformSDL3>();

    auto SDL3 = platform_api<FrenchieApplicationPlatformSDL3>();

#ifdef FRENCHIE_APPLICATION_PLATFORM_IS_UNIX
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

    // create context
    SDL3->Window =
        SDL_CreateWindow("Application", 512, 256, SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);

    if(m_Api->Window == nullptr)
    {
        SDL_Quit();
        return false;
    }

    SDL3->Context = SDL_GL_CreateContext(reinterpret_cast<SDL_Window*>(SDL3->Window));

    if(SDL3->Context == nullptr)
    {
        SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(SDL3->Window));
        SDL_Quit();
        return false;
    }

    // configure context
    if(!SDL_GL_MakeCurrent(reinterpret_cast<SDL_Window*>(SDL3->Window), SDL3->Context))
    {
        SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(SDL3->Window));
        SDL_GL_DestroyContext(SDL3->Context);
        SDL_Quit();
        return false;
    }

    // load rendering backend
    if(!ApplicationRenderingBackend::awake((void*(*)(const char*))SDL_GL_GetProcAddress))
    {
        SDL_Quit();
        return false;
    }

    return true;
}