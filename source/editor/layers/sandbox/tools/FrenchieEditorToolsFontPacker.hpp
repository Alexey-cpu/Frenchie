#pragma once

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationLayer.hpp>

namespace Frenchie
{
    namespace Editor
    {
        class FontPackerTool : public Frenchie::Application::Layer
        {
        public:
            FontPackerTool();
            virtual ~FontPackerTool();

            // Frenchie::Application::Layer
            virtual void frame_update() override;

        protected:
            void load_fonts();
        };
    }
}