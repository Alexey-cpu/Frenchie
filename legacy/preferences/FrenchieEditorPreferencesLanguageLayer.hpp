#pragma once

#include <FrenchieApplicationLayer.hpp>

namespace Frenchie
{
    namespace Editor
    {
        namespace Preferences
        {
            class Languages : 
                public Frenchie::Application::Layer::Registry<Languages>
            {
            public:
                Languages();
                virtual ~Languages();

                // Frenchie::Application::Layer::Registry<Localizations>
                static std::string factory_id();

                // Frenchie::Application::Layer
                virtual void frame_update();
                virtual bool allows_multiple_instances() const;
            };

            const bool preferencesLanguageRegistry = Languages::registerFactory();
        }
    }
}