#include <FrenchieApplicationEditorPreferencesStyleLayer.hpp>

#include <FrenchieApplication.hpp>
#include <FrenchieApplicationEditorCommandsLayer.hpp>
#include <FrenchieApplicationEditorConfigurationFontsLayer.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>
#include <FrenchieApplicationEditorConfigurationTranslatorLayer.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_internal.h>

static const char* GetTreeLinesFlagsName(ImGuiTreeNodeFlags flags)
{
    if (flags == ImGuiTreeNodeFlags_DrawLinesNone) return "DrawLinesNone";
    if (flags == ImGuiTreeNodeFlags_DrawLinesFull) return "DrawLinesFull";
    if (flags == ImGuiTreeNodeFlags_DrawLinesToNodes) return "DrawLinesToNodes";
    return "";
}

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;
using namespace Frenchie::Application::Editor::Preferences;
using namespace Frenchie::Application::Editor::Configuration;

// Style
Style::Style() : 
    Frenchie::Application::Layer::Registry<Style>(
        Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE")){}

Style::~Style(){}

std::string Style::factory_id()
{
    return STRINGIFY(Frenchie::Application::Editor::Preferences::Style);
}

bool Style::awake()
{
    m_ReferenceStyle = ImGui::GetStyle();
    return true;
}

void Style::frame_update() 
{
    ImGui::BeginChild(get_name().c_str());
    draw_style_editor();
    ImGui::EndChild();
}

bool Style::allows_multiple_instances() const
{
    return false;
}

void Style::draw_style_editor()
{
    if (ImGui::TreeNode(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_GEOMETRY").c_str()))
    {
        draw_geometry_settings();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_COLORS").c_str()))
    {
        draw_color_settings();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_FONTS").c_str()))
    {
        draw_fonts_settings();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_RENDERING").c_str()))
    {
        draw_rendering_settings();
        ImGui::TreePop();
    }
}

void Style::draw_geometry_settings()
{
    auto& style = ImGui::GetStyle();

    ImGui::SeparatorText(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_GEOMETRY_MAIN").c_str());
    {
        ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
        ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
        ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
        ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
    }

    ImGui::SeparatorText(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_GEOMETRY_BORDERS").c_str());
    {
        {
            bool checked = style.WindowBorderSize > 0.0;
            if(ImGui::Checkbox("WindowBorder", &checked))
                style.WindowBorderSize = (float)(checked);
        }

        {
            bool checked = style.ChildBorderSize > 0.0;
            if(ImGui::Checkbox("ChildBorder", &checked)) 
                style.ChildBorderSize = (float)(checked);
        }

        {
            bool checked = style.PopupBorderSize > 0.0;
            if(ImGui::Checkbox("PopupBorder", &checked)) 
                style.PopupBorderSize = (float)(checked);
        }

        {
            bool checked = style.FrameBorderSize > 0.0;
            if(ImGui::Checkbox("FrameBorder", &checked))
                style.FrameBorderSize = (float)(checked);
        }

        {
            bool checked = style.TabBorderSize > 0.0f;
            if(ImGui::Checkbox("TabBorder", &checked))
                style.TabBorderSize = (float)(checked);
        }

        {
            bool checked = style.TabBarBorderSize > 0.0f;
            if(ImGui::Checkbox("TabBarBorder", &checked))
                style.TabBarBorderSize = (float)(checked);
        }
    }

    ImGui::SeparatorText(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_GEOMETRY_ROUNDING").c_str());
    {
        ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
    }

    ImGui::SeparatorText(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_GEOMETRY_TABS").c_str());
    {
        ImGui::SliderFloat("TabBarOverlineSize", &style.TabBarOverlineSize, 0.0f, 3.0f, "%.0f");
        ImGui::DragFloat("TabMinWidthBase", &style.TabMinWidthBase, 0.5f, 1.0f, 500.0f, "%.0f");
        ImGui::DragFloat("TabMinWidthShrink", &style.TabMinWidthShrink, 0.5f, 1.0f, 500.0f, "%0.f");

        {
            bool checked = style.TabCloseButtonMinWidthSelected < 0.0f;
            if(ImGui::Checkbox("ShowCloseButtonOnSelectedTab", &checked))
                style.TabCloseButtonMinWidthSelected = (float)(checked) * (-1.f);
        }

        {
            bool checked = style.TabCloseButtonMinWidthUnselected < 0.0f;
            if(ImGui::Checkbox("ShowCloseButtonOnUnselectedTab", &checked))
                style.TabCloseButtonMinWidthUnselected = (float)(checked) * (-1.f); 
        }
    }

    ImGui::SeparatorText(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_GEOMETRY_TABLES").c_str());
    {
        ImGui::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2("TableAngledHeadersTextAlign", (float*)&style.TableAngledHeadersTextAlign, 0.0f, 1.0f, "%.2f");
        ImGui::SliderAngle("TableAngledHeadersAngle", &style.TableAngledHeadersAngle, -50.0f, +50.0f);
    }

    ImGui::SeparatorText(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_GEOMETRY_TREES").c_str());
    {
        if (ImGui::BeginCombo("TreeLinesFlags", GetTreeLinesFlagsName(style.TreeLinesFlags)))
        {
            const ImGuiTreeNodeFlags options[] = 
            { 
                ImGuiTreeNodeFlags_DrawLinesNone, 
                ImGuiTreeNodeFlags_DrawLinesFull, 
                ImGuiTreeNodeFlags_DrawLinesToNodes 
            };
            
            for(ImGuiTreeNodeFlags option : options)
            {
                if (ImGui::Selectable(GetTreeLinesFlagsName(option), style.TreeLinesFlags == option))
                    style.TreeLinesFlags = option;
            }

            ImGui::EndCombo();
        }
        ImGui::SliderFloat("TreeLinesSize", &style.TreeLinesSize, 0.0f, 2.0f, "%.0f");
        ImGui::SliderFloat("TreeLinesRounding", &style.TreeLinesRounding, 0.0f, 12.0f, "%.0f");
    }

    ImGui::SeparatorText(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_GEOMETRY_DOCKING").c_str());
    {
        ImGui::SliderFloat("DockingSeparatorSize", &style.DockingSeparatorSize, 0.0f, 12.0f, "%.0f");
    }
}

void Style::draw_color_settings()
{
    auto& style = ImGui::GetStyle();

    if (ImGui::ShowStyleSelector("Colors##Selector")){}

    static ImGuiTextFilter filter;
    filter.Draw(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_COLORS_FILTER").c_str(), ImGui::GetFontSize() * 16);

    static ImGuiColorEditFlags alpha_flags = 0;
    if (ImGui::RadioButton(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_COLORS_OPAQUE").c_str(), alpha_flags == ImGuiColorEditFlags_AlphaOpaque))
        alpha_flags = ImGuiColorEditFlags_AlphaOpaque;
    ImGui::SameLine();
    
    if (ImGui::RadioButton(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_COLORS_ALPHA").c_str(), alpha_flags == ImGuiColorEditFlags_None))
        alpha_flags = ImGuiColorEditFlags_None;
    ImGui::SameLine();
    
    if (ImGui::RadioButton(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_COLORS_BOTH").c_str(), alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf))
        alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf;

    ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::BeginChild("##colors", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
    ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
    for (int i = 0; i < ImGuiCol_COUNT; i++)
    {
        //const char* name = ImGui::GetStyleColorName(i);

        std::string name = 
            fmt::format(
                Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_COLORS_{}").c_str(), 
                Frenchie::Core::String::to_upper(ImGui::GetStyleColorName(i)));
        
        if (!filter.PassFilter(name.c_str()))
            continue;
        ImGui::PushID(i);
        
        ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
        
        // apply / revert functionality...
        if (memcmp(&style.Colors[i], &m_ReferenceStyle.Colors[i], sizeof(ImVec4)) != 0)
        {
            ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); 
            
            if(ImGui::Button(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_COLORS_SAVE").c_str())) 
                m_ReferenceStyle.Colors[i] = style.Colors[i];
            
            ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); 
            
            if (ImGui::Button(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_COLORS_REVERT").c_str())) 
                style.Colors[i] = m_ReferenceStyle.Colors[i];
        }

        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        ImGui::TextUnformatted(name.c_str());
        ImGui::PopID();
    }
    ImGui::PopItemWidth();
    ImGui::EndChild();
}

void Style::draw_fonts_settings()
{
    // font selector
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font_current = ImGui::GetFont();

    if (ImGui::BeginCombo("##", font_current->GetDebugName()))
    {
        for (ImFont* font : io.Fonts->Fonts)
        {
            ImGui::PushID((void*)font);
            if (ImGui::Selectable(font->GetDebugName(), font == font_current))
                io.FontDefault = font;
            if (font == font_current)
                ImGui::SetItemDefaultFocus();
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();

    if(ImGui::Button(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_FONTS_BROWSE_BUTTON").c_str()))
    {
        Frenchie::Application::Application::instance()->push<FileSystem::Dialogs::ExplorerDialog>(
            Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_FONTS_SEARCH_DIALOG_TITLE").c_str(),
            [this]()
            {
                Reference<FileSystem::Dialogs::ExplorerDialog> dialog = 
                    Frenchie::Application::Application::instance()->find<FileSystem::Dialogs::ExplorerDialog>();

                if(dialog != nullptr)
                {
                    auto path = dialog->get_path();

                    Frenchie::Application::Application::instance()->find_or_push<CommandsQueue>()->push<CallbackCommand>(
                        [this, path]()
                        {
                            Frenchie::Application::Application::instance()->find_or_push<Configuration::Fonts>()->scan_fonts(path);
                        }
                    );
                }
            }
        );
    }

    // font size
    if (ImGui::DragFloat(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_FONTS_SIZE").c_str(), &ImGui::GetStyle().FontSizeBase, 0.20f, 5.0f, 100.0f, "%.0f"))
        ImGui::GetStyle()._NextFrameFontSizeBase = ImGui::GetStyle().FontSizeBase;
}

void Style::draw_rendering_settings()
{
    auto& style = ImGui::GetStyle();

    ImGui::Checkbox(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_RENDERING_ANTIALIASED_LINES").c_str(), &style.AntiAliasedLines);
    ImGui::Checkbox(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_RENDERING_ANTIALIASED_LINES_USE_TEXTURE").c_str(), &style.AntiAliasedLinesUseTex);
    ImGui::Checkbox(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_RENDERING_ANTIALIASED_FILL").c_str(), &style.AntiAliasedFill);
    ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
    ImGui::DragFloat(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_RENDERING_CURVE_TESSELLATION_TOLERANCE").c_str(), &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
    if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;

    // When editing the "Circle Segment Max Error" value, draw a preview of its effect on auto-tessellated circles.
    ImGui::DragFloat(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_RENDERING_CIRCLE_TESSELLATION_MAX_ERROR").c_str(), &style.CircleTessellationMaxError , 0.005f, 0.10f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    const bool show_samples = ImGui::IsItemActive();
    if (show_samples)
        ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());

    if (show_samples && ImGui::BeginTooltip())
    {
        ImGui::TextUnformatted("(R = radius, N = approx number of segments)");
        ImGui::Spacing();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const float min_widget_width = ImGui::CalcTextSize("R: MMM\nN: MMM").x;
        for (int n = 0; n < 8; n++)
        {
            const float RAD_MIN = 5.0f;
            const float RAD_MAX = 70.0f;
            const float rad = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (8.0f - 1.0f);

            ImGui::BeginGroup();

            // N is not always exact here due to how PathArcTo() function work internally
            ImGui::Text("R: %.f\nN: %d", rad, draw_list->_CalcCircleAutoSegmentCount(rad));

            const float canvas_width = std::max<float>(min_widget_width, rad * 2.0f);
            const float offset_x     = floorf(canvas_width * 0.5f);
            const float offset_y     = floorf(RAD_MAX);

            const ImVec2 p1 = ImGui::GetCursorScreenPos();
            draw_list->AddCircle(ImVec2(p1.x + offset_x, p1.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
            ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));
            ImGui::EndGroup();
            ImGui::SameLine();
        }
        ImGui::EndTooltip();
    }

    ImGui::DragFloat(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_RENDERING_GLOBAL_ALPHA").c_str(), &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
    ImGui::DragFloat(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_STYLE_TAB_RENDERING_DISABLED_ALPHA").c_str(), &style.DisabledAlpha, 0.005f, 0.0f, 1.0f, "%.2f"); 
    ImGui::PopItemWidth();
}