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
            namespace Preferences
            {
                class Style : 
                    public Frenchie::Application::Layer::Registry<Style>
                {
                public:
                    Style();
                    virtual ~Style();

                    // Frenchie::Application::Layer::Registry<Style>
                    static std::string factory_id();

                    // Frenchie::Application::Layer
                    virtual bool awake() override;
                    virtual void frame_update() override;
                    virtual bool allows_multiple_instances() const override;

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
}