#pragma once

#include <FrenchieApplicationEditorPreferencesLayer.hpp>
#include <FrenchieApplicationEditorInputText.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class EnvironmentSettings : 
                public Frenchie::Application::Layer::Registry<EnvironmentSettings>, 
                public Frenchie::Core::Serialization::ISerializer
            {
            public:
                EnvironmentSettings();
                virtual ~EnvironmentSettings();

                // Frenchie::Application::Layer::Registry<EnvironmentSettings>
                static std::string factory_id();

                // Frenchie::Application::Layer
                virtual void frame_update() override;

                // Frenchie::Core::Serialization::ISerializer
                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

            protected:

                std::string m_PATH    = std::string();
                std::string m_LIB     = std::string();
                std::string m_INCLUDE = std::string();

                void draw_editor_folders();
                void draw_system_path_variable();
                void draw_system_lib_variable();
                void draw_system_include_variable();
            };
        }
    }
}