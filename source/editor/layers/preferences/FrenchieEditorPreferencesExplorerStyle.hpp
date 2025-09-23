#pragma once

#include <FrenchieApplicationLayer.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_internal.h>

namespace Frenchie
{
    namespace Editor
    {
        namespace Preferences
        {
            class Style :
                public Frenchie::Application::Layer
            {
            public:
                Style();
                virtual ~Style();

                // Frenchie::Application::Layer
                virtual bool awake() override;
                virtual void frame_update() override;
                virtual bool allows_multiple_instances() const override;

            protected:

                // info
                ImGuiStyle      m_ReferenceStyle;
                ImGuiTextFilter m_ColorFilter;

                void draw_geometry_settings();
                void draw_color_settings();
                void draw_fonts_settings();
                void draw_rendering_settings();

                // service methods
                std::string get_style_color_name(ImGuiCol);
                const char* GetTreeLinesFlagsName(ImGuiTreeNodeFlags flags);
            };
        }
    }
}