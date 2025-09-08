#pragma once

#include <FrenchieApplicationLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            namespace Preferences
            {
                class Localizations : 
                    public Frenchie::Application::Layer::Registry<Localizations>
                {
                public:
                    Localizations();
                    virtual ~Localizations();

                    // Frenchie::Application::Layer::Registry<Localizations>
                    static std::string factory_id();

                    // Frenchie::Application::Layer
                    virtual void frame_update();
                    virtual bool allows_multiple_instances() const;
                };
            }
        }
    }
}