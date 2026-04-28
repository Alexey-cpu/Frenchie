#pragma once

// Application
#include <FrenchieApplicationPlatformBackend.hpp>

// SDL3
#include "SDL3/SDL.h"

namespace Frenchie
{
    namespace Application
    {
        struct FrenchieApplicationPlatformSDL3 : public FrenchieApplicationPlatformApi
        {
            FrenchieApplicationPlatformSDL3(){}
            virtual ~FrenchieApplicationPlatformSDL3(){}

            SDL_Event     Event;
            SDL_GLContext Context;
        };
    }
}