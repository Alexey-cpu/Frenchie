#pragma once

#include <FrenchieApplicationEditorPreferencesLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class KitsSettings : 
                public Frenchie::Application::Layer::Registry<KitsSettings>, 
                public Frenchie::Core::Serialization::ISerializer
            {
            public:
                KitsSettings();
                virtual ~KitsSettings();

                // Frenchie::Application::Layer::Registry<KitsSettings>
                static std::string factory_id();

                // Frenchie::Application::Layer
                virtual void frame_update() override;

                // Frenchie::Core::Serialization::ISerializer
                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;
            };
        }
    }
}