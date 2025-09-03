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
                ImGuiStyle m_ReferenceStyle;

                void draw_style_editor();
                void draw_geometry_settings(ImGuiStyle&);
                void draw_color_settings(ImGuiStyle&);
                void draw_fonts_settings(ImGuiStyle&);
                void draw_rendering_settings(ImGuiStyle&);

                ImGuiCol_ get_imgui_color(std::string _Name)
                {
                    if(_Name == STRINGIFY(ImGuiCol_Text)) 
                        return ImGuiCol_Text;
                    if(_Name == STRINGIFY(ImGuiCol_WindowBg)) 
                        return ImGuiCol_WindowBg;
                    if(_Name == STRINGIFY(ImGuiCol_ChildBg)) 
                        return ImGuiCol_ChildBg;
                    if(_Name == STRINGIFY(ImGuiCol_PopupBg)) 
                        return ImGuiCol_PopupBg;
                    if(_Name == STRINGIFY(ImGuiCol_Border)) 
                        return ImGuiCol_Border;
                    if(_Name == STRINGIFY(ImGuiCol_BorderShadow)) 
                        return ImGuiCol_BorderShadow;
                    if(_Name == STRINGIFY(ImGuiCol_FrameBg)) 
                        return ImGuiCol_FrameBg;
                    if(_Name == STRINGIFY(ImGuiCol_FrameBgHovered)) 
                        return ImGuiCol_FrameBgHovered;
                    if(_Name == STRINGIFY(ImGuiCol_FrameBgActive)) 
                        return ImGuiCol_FrameBgActive;
                    if(_Name == STRINGIFY(ImGuiCol_TitleBg)) 
                        return ImGuiCol_TitleBg;
                    if(_Name == STRINGIFY(ImGuiCol_TitleBgActive)) 
                        return ImGuiCol_TitleBgActive;
                    if(_Name == STRINGIFY(ImGuiCol_TitleBgCollapsed)) 
                        return ImGuiCol_TitleBgCollapsed;
                    if(_Name == STRINGIFY(ImGuiCol_MenuBarBg)) 
                        return ImGuiCol_MenuBarBg;
                    if(_Name == STRINGIFY(ImGuiCol_ScrollbarBg)) 
                        return ImGuiCol_ScrollbarBg;
                    if(_Name == STRINGIFY(ImGuiCol_ScrollbarGrab)) 
                        return ImGuiCol_ScrollbarGrab;
                    if(_Name == STRINGIFY(ImGuiCol_ScrollbarGrabHovered)) 
                        return ImGuiCol_ScrollbarGrabHovered;
                    if(_Name == STRINGIFY(ImGuiCol_ScrollbarGrabActive)) 
                        return ImGuiCol_ScrollbarGrabActive;
                    if(_Name == STRINGIFY(ImGuiCol_CheckMark)) 
                        return ImGuiCol_CheckMark;
                    if(_Name == STRINGIFY(ImGuiCol_SliderGrab)) 
                        return ImGuiCol_SliderGrab;
                    if(_Name == STRINGIFY(ImGuiCol_SliderGrabActive)) 
                        return ImGuiCol_SliderGrabActive;
                    if(_Name == STRINGIFY(ImGuiCol_Button)) 
                        return ImGuiCol_Button;
                    if(_Name == STRINGIFY(ImGuiCol_ButtonHovered)) 
                        return ImGuiCol_ButtonHovered;
                    if(_Name == STRINGIFY(ImGuiCol_ButtonActive)) 
                        return ImGuiCol_ButtonActive;
                    if(_Name == STRINGIFY(ImGuiCol_Header)) 
                        return ImGuiCol_Header;
                    if(_Name == STRINGIFY(ImGuiCol_HeaderHovered)) 
                        return ImGuiCol_HeaderHovered;
                    if(_Name == STRINGIFY(ImGuiCol_HeaderActive)) 
                        return ImGuiCol_HeaderActive;
                    if(_Name == STRINGIFY(ImGuiCol_Separator)) 
                        return ImGuiCol_Separator;
                    if(_Name == STRINGIFY(ImGuiCol_SeparatorHovered)) 
                        return ImGuiCol_SeparatorHovered;
                    if(_Name == STRINGIFY(ImGuiCol_SeparatorActive)) 
                        return ImGuiCol_SeparatorActive;
                    if(_Name == STRINGIFY(ImGuiCol_ResizeGrip)) 
                        return ImGuiCol_ResizeGrip;
                    if(_Name == STRINGIFY(ImGuiCol_ResizeGripHovered)) 
                        return ImGuiCol_ResizeGripHovered;
                    if(_Name == STRINGIFY(ImGuiCol_ResizeGripActive)) 
                        return ImGuiCol_ResizeGripActive;
                    if(_Name == STRINGIFY(ImGuiCol_Tab)) 
                        return ImGuiCol_Tab;
                    if(_Name == STRINGIFY(ImGuiCol_TabHovered)) 
                        return ImGuiCol_TabHovered;


                    if(_Name == STRINGIFY(ImGuiCol_DockingPreview)) 
                        return ImGuiCol_DockingPreview;
                    if(_Name == STRINGIFY(ImGuiCol_DockingEmptyBg)) 
                        return ImGuiCol_DockingEmptyBg;
                    if(_Name == STRINGIFY(ImGuiCol_PlotLines)) 
                        return ImGuiCol_PlotLines;
                    if(_Name == STRINGIFY(ImGuiCol_PlotLinesHovered)) 
                        return ImGuiCol_PlotLinesHovered;
                    if(_Name == STRINGIFY(ImGuiCol_PlotHistogram)) 
                        return ImGuiCol_PlotHistogram;
                    if(_Name == STRINGIFY(ImGuiCol_PlotHistogramHovered)) 
                        return ImGuiCol_PlotHistogramHovered;
                    if(_Name == STRINGIFY(ImGuiCol_TableHeaderBg)) 
                        return ImGuiCol_TableHeaderBg;
                    if(_Name == STRINGIFY(ImGuiCol_TableBorderStrong)) 
                        return ImGuiCol_TableBorderStrong;
                    if(_Name == STRINGIFY(ImGuiCol_TableBorderLight)) 
                        return ImGuiCol_TableBorderLight;
                    if(_Name == STRINGIFY(ImGuiCol_TableRowBg)) 
                        return ImGuiCol_TableRowBg;
                    if(_Name == STRINGIFY(ImGuiCol_TableRowBgAlt)) 
                        return ImGuiCol_TableRowBgAlt;
                    if(_Name == STRINGIFY(ImGuiCol_TextSelectedBg)) 
                        return ImGuiCol_TextSelectedBg;
                    if(_Name == STRINGIFY(ImGuiCol_DragDropTarget)) 
                        return ImGuiCol_DragDropTarget;

                    if(_Name == STRINGIFY(ImGuiCol_NavWindowingHighlight)) 
                        return ImGuiCol_NavWindowingHighlight;
                    if(_Name == STRINGIFY(ImGuiCol_NavWindowingDimBg)) 
                        return ImGuiCol_NavWindowingDimBg;
                    if(_Name == STRINGIFY(ImGuiCol_ModalWindowDimBg)) 
                        return ImGuiCol_ModalWindowDimBg;
                    return ImGuiCol_Text; // Default fallback
                }

                std::string get_imgui_color_name(ImGuiCol_ _Color)
                {
                    Frenchie::Core::Logger::instance()->error(Frenchie::Core::Helpers::String::to_string((int)(_Color)));

                    switch (_Color)
                    {
                        case ImGuiCol_::ImGuiCol_Text:
                            return STRINGIFY(ImGuiCol_Text);
                        case ImGuiCol_::ImGuiCol_TextDisabled:
                            return STRINGIFY(ImGuiCol_TextDisabled);
                        case ImGuiCol_::ImGuiCol_WindowBg:
                            return STRINGIFY(ImGuiCol_WindowBg);
                        case ImGuiCol_::ImGuiCol_ChildBg:
                            return STRINGIFY(ImGuiCol_ChildBg);
                        case ImGuiCol_::ImGuiCol_PopupBg:
                            return STRINGIFY(ImGuiCol_PopupBg);
                        case ImGuiCol_::ImGuiCol_Border:
                            return STRINGIFY(ImGuiCol_Border);
                        case ImGuiCol_::ImGuiCol_BorderShadow:
                            return STRINGIFY(ImGuiCol_BorderShadow);
                        case ImGuiCol_::ImGuiCol_FrameBg:
                            return STRINGIFY(ImGuiCol_FrameBg);
                        case ImGuiCol_::ImGuiCol_FrameBgHovered:
                            return STRINGIFY(ImGuiCol_FrameBgHovered);
                        case ImGuiCol_::ImGuiCol_FrameBgActive:
                            return STRINGIFY(ImGuiCol_FrameBgActive);
                        case ImGuiCol_::ImGuiCol_TitleBg:
                            return STRINGIFY(ImGuiCol_TitleBg);
                        case ImGuiCol_::ImGuiCol_TitleBgActive:
                            return STRINGIFY(ImGuiCol_TitleBgActive);
                        case ImGuiCol_::ImGuiCol_TitleBgCollapsed:
                            return STRINGIFY(ImGuiCol_TitleBgCollapsed);
                        case ImGuiCol_::ImGuiCol_MenuBarBg:
                            return STRINGIFY(ImGuiCol_MenuBarBg);
                        case ImGuiCol_::ImGuiCol_ScrollbarBg:
                            return STRINGIFY(ImGuiCol_ScrollbarBg);
                        case ImGuiCol_::ImGuiCol_ScrollbarGrab:
                            return STRINGIFY(ImGuiCol_ScrollbarGrab);
                        case ImGuiCol_::ImGuiCol_ScrollbarGrabHovered:
                            return STRINGIFY(ImGuiCol_ScrollbarGrabHovered);
                        case ImGuiCol_::ImGuiCol_ScrollbarGrabActive:
                            return STRINGIFY(ImGuiCol_ScrollbarGrabActive);
                        case ImGuiCol_::ImGuiCol_CheckMark:
                            return STRINGIFY(ImGuiCol_CheckMark);
                        case ImGuiCol_::ImGuiCol_SliderGrab:
                            return STRINGIFY(ImGuiCol_SliderGrab);
                        case ImGuiCol_::ImGuiCol_SliderGrabActive:
                            return STRINGIFY(ImGuiCol_SliderGrabActive);
                        case ImGuiCol_::ImGuiCol_Button:
                            return STRINGIFY(ImGuiCol_Button);
                        case ImGuiCol_::ImGuiCol_ButtonHovered:
                            return STRINGIFY(ImGuiCol_ButtonHovered);
                        case ImGuiCol_::ImGuiCol_ButtonActive:
                            return STRINGIFY(ImGuiCol_ButtonActive);
                        case ImGuiCol_::ImGuiCol_Header:
                            return STRINGIFY(ImGuiCol_Header);
                        case ImGuiCol_::ImGuiCol_HeaderHovered:
                            return STRINGIFY(ImGuiCol_HeaderHovered);
                        case ImGuiCol_::ImGuiCol_HeaderActive:
                            return STRINGIFY(ImGuiCol_HeaderActive);
                        case ImGuiCol_::ImGuiCol_Separator:
                            return STRINGIFY(ImGuiCol_Separator);
                        case ImGuiCol_::ImGuiCol_SeparatorHovered:
                            return STRINGIFY(ImGuiCol_SeparatorHovered);
                        case ImGuiCol_::ImGuiCol_SeparatorActive:
                            return STRINGIFY(ImGuiCol_SeparatorActive);
                        case ImGuiCol_::ImGuiCol_ResizeGrip:
                            return STRINGIFY(ImGuiCol_ResizeGrip);
                        case ImGuiCol_::ImGuiCol_ResizeGripHovered:
                            return STRINGIFY(ImGuiCol_ResizeGripHovered);
                        case ImGuiCol_::ImGuiCol_ResizeGripActive:
                            return STRINGIFY(ImGuiCol_ResizeGripActive);
                        case ImGuiCol_::ImGuiCol_InputTextCursor:
                            return STRINGIFY(ImGuiCol_InputTextCursor);
                        case ImGuiCol_::ImGuiCol_TabHovered:
                            return STRINGIFY(ImGuiCol_TabHovered);
                        case ImGuiCol_::ImGuiCol_Tab:
                            return STRINGIFY(ImGuiCol_Tab);
                        case ImGuiCol_::ImGuiCol_TabSelected:
                            return STRINGIFY(ImGuiCol_TabSelected);
                        case ImGuiCol_::ImGuiCol_TabSelectedOverline:
                            return STRINGIFY(ImGuiCol_TabSelectedOverline);
                        case ImGuiCol_::ImGuiCol_TabDimmed:
                            return STRINGIFY(ImGuiCol_TabDimmed);
                        case ImGuiCol_::ImGuiCol_TabDimmedSelected:
                            return STRINGIFY(ImGuiCol_TabDimmedSelected);
                        case ImGuiCol_::ImGuiCol_TabDimmedSelectedOverline:
                            return STRINGIFY(ImGuiCol_TabDimmedSelectedOverline);
                        case ImGuiCol_::ImGuiCol_DockingPreview:
                            return STRINGIFY(ImGuiCol_DockingPreview);
                        case ImGuiCol_::ImGuiCol_DockingEmptyBg:
                            return STRINGIFY(ImGuiCol_DockingEmptyBg);
                        case ImGuiCol_::ImGuiCol_PlotLines:
                            return STRINGIFY(ImGuiCol_PlotLines);
                        case ImGuiCol_::ImGuiCol_PlotLinesHovered:
                            return STRINGIFY(ImGuiCol_PlotLinesHovered);
                        case ImGuiCol_::ImGuiCol_PlotHistogram:
                            return STRINGIFY(ImGuiCol_PlotHistogram);
                        case ImGuiCol_::ImGuiCol_PlotHistogramHovered:
                            return STRINGIFY(ImGuiCol_PlotHistogramHovered);
                        case ImGuiCol_::ImGuiCol_TableHeaderBg:
                            return STRINGIFY(ImGuiCol_TableHeaderBg);
                        case ImGuiCol_::ImGuiCol_TableBorderStrong:
                            return STRINGIFY(ImGuiCol_TableBorderStrong);
                        case ImGuiCol_::ImGuiCol_TableBorderLight:
                            return STRINGIFY(ImGuiCol_TableBorderLight);
                        case ImGuiCol_::ImGuiCol_TableRowBg:
                            return STRINGIFY(ImGuiCol_TableRowBg);
                        case ImGuiCol_::ImGuiCol_TableRowBgAlt:
                            return STRINGIFY(ImGuiCol_TableRowBgAlt);
                        case ImGuiCol_::ImGuiCol_TextLink:
                            return STRINGIFY(ImGuiCol_TextLink);
                        case ImGuiCol_::ImGuiCol_TextSelectedBg:
                            return STRINGIFY(ImGuiCol_TextSelectedBg);
                        case ImGuiCol_::ImGuiCol_TreeLines:
                            return STRINGIFY(ImGuiCol_TreeLines);
                        case ImGuiCol_::ImGuiCol_DragDropTarget:
                            return STRINGIFY(ImGuiCol_DragDropTarget);
                        case ImGuiCol_::ImGuiCol_NavHighlight:
                            return STRINGIFY(ImGuiCol_NavHighlight);
                        case ImGuiCol_::ImGuiCol_NavWindowingHighlight:
                            return STRINGIFY(ImGuiCol_NavWindowingHighlight);
                        case ImGuiCol_::ImGuiCol_NavWindowingDimBg:
                            return STRINGIFY(ImGuiCol_NavWindowingDimBg);
                        case ImGuiCol_::ImGuiCol_ModalWindowDimBg:
                            return STRINGIFY(ImGuiCol_ModalWindowDimBg);
                    }

                    Frenchie::Core::Logger::instance()->error(fmt::format("{} --> {}", (int)_Color, "Unknown"));

                    return "Unknown";
                }
            };
        }
    }
}