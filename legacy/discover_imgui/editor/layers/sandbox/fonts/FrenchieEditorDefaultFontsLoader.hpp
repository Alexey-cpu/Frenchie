#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Configuration
        {
            class DefaultFonts : public Frenchie::Application::Layer
            {
            public:
                DefaultFonts();
                virtual ~DefaultFonts();

                virtual bool awake() override;
                virtual void frame_update() override;
            };
        }
    }
}