#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>

namespace Frenchie
{
    namespace Editor
    {
        class DefaultFontsLoader : public Frenchie::Application::Layer
        {
        public:
            DefaultFontsLoader();
            virtual ~DefaultFontsLoader();

            virtual bool awake() override;
            virtual void frame_update() override;
        };
    }
}