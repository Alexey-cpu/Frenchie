#pragma once

#include <FrenchieApplicationEditorPreferencesLayer.hpp>

// IMGUI
#include <imgui.h>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class StyleSettings : 
                public Frenchie::Application::Layer::Registry<StyleSettings>, 
                public Frenchie::Core::Serialization::ISerializer
            {
            public:
                StyleSettings();
                virtual ~StyleSettings();

                // Frenchie::Application::Layer::Registry<EnvironmentSettings>
                static std::string factory_id();

                // Frenchie::Application::Layer
                virtual bool awake() override;
                virtual void frame_update() override;

                // Frenchie::Core::Serialization::ISerializer
                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

            protected:

                // info
                ImGuiStyle            m_ReferenceStyle;
                std::filesystem::path m_FontsLoadPath = std::filesystem::current_path();

                void draw_style_editor();
                void draw_geometry_settings();
                void draw_color_settings();
                void draw_fonts_settings();
                void draw_rendering_settings();

                void load_fonts(const std::filesystem::path&);
            };
        }
    }
}