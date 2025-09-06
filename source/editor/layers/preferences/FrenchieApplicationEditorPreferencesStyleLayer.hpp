#pragma once

#include <FrenchieApplicationEditorPreferencesLayer.hpp>

#include <FrenchieApplicationEditorInputText.hpp>

// IMGUI
#include <imgui.h>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class Style : 
                public Frenchie::Application::Layer::Registry<Style>
            {
            public:
                Style();
                virtual ~Style();

                // Frenchie::Application::Editor::Preferences::Topic::Registry<Style>
                static std::string factory_id();

                // Frenchie::Application::Editor::Preferences::Topic
                virtual bool awake() override;
                virtual void frame_update() override;

            protected:

                // info
                ImGuiStyle m_ReferenceStyle;
                InputText  m_FontsLocation;

                void draw_style_editor();
                void draw_geometry_settings();
                void draw_color_settings();
                void draw_fonts_settings();
                void draw_rendering_settings();
            };
        }
    }
}