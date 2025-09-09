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
        Translator::translate("Style")){}

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
    if (ImGui::TreeNode(Translator::translate("Geometry").c_str()))
    {
        draw_geometry_settings();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode(Translator::translate("Colors").c_str()))
    {
        draw_color_settings();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode(Translator::translate("Fonts").c_str()))
    {
        draw_fonts_settings();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode(Translator::translate("Rendering").c_str()))
    {
        draw_rendering_settings();
        ImGui::TreePop();
    }
}

void Style::draw_geometry_settings()
{
    auto& style = ImGui::GetStyle();

    ImGui::SeparatorText(Translator::translate("Main").c_str());
    {
        ImGui::SliderFloat2(Translator::translate("WindowPadding").c_str(), (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2(Translator::translate("FramePadding").c_str(), (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2(Translator::translate("ItemSpacing").c_str(), (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2(Translator::translate("ItemInnerSpacing").c_str(), (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2(Translator::translate("TouchExtraPadding").c_str(), (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
        ImGui::SliderFloat(Translator::translate("IndentSpacing").c_str(), &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
        ImGui::SliderFloat(Translator::translate("ScrollbarSize").c_str(), &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
        ImGui::SliderFloat(Translator::translate("GrabMinSize").c_str(), &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
    }

    ImGui::SeparatorText(Translator::translate("Borders").c_str());
    {
        {
            bool checked = style.WindowBorderSize > 0.0;
            if(ImGui::Checkbox(Translator::translate("WindowBorder").c_str(), &checked))
                style.WindowBorderSize = (float)(checked);
        }

        {
            bool checked = style.ChildBorderSize > 0.0;
            if(ImGui::Checkbox(Translator::translate("ChildBorder").c_str(), &checked)) 
                style.ChildBorderSize = (float)(checked);
        }

        {
            bool checked = style.PopupBorderSize > 0.0;
            if(ImGui::Checkbox(Translator::translate("PopupBorder").c_str(), &checked)) 
                style.PopupBorderSize = (float)(checked);
        }

        {
            bool checked = style.FrameBorderSize > 0.0;
            if(ImGui::Checkbox(Translator::translate("FrameBorder").c_str(), &checked))
                style.FrameBorderSize = (float)(checked);
        }

        {
            bool checked = style.TabBorderSize > 0.0f;
            if(ImGui::Checkbox(Translator::translate("TabBorder").c_str(), &checked))
                style.TabBorderSize = (float)(checked);
        }

        {
            bool checked = style.TabBarBorderSize > 0.0f;
            if(ImGui::Checkbox("TabBarBorder", &checked))
                style.TabBarBorderSize = (float)(checked);
        }
    }

    ImGui::SeparatorText(Translator::translate(Translator::translate("Rounding").c_str()).c_str());
    {
        ImGui::SliderFloat(Translator::translate("WindowRounding").c_str(), &style.WindowRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat(Translator::translate("ChildRounding").c_str(), &style.ChildRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat(Translator::translate("FrameRounding").c_str(), &style.FrameRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat(Translator::translate("PopupRounding").c_str(), &style.PopupRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat(Translator::translate("ScrollbarRounding").c_str(), &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat(Translator::translate("GrabRounding").c_str(), &style.GrabRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat(Translator::translate("TabRounding").c_str(), &style.TabRounding, 0.0f, 12.0f, "%.0f");
    }

    ImGui::SeparatorText(Translator::translate("Tabs").c_str());
    {
        ImGui::SliderFloat(Translator::translate("TabBarOverlineSize").c_str(), &style.TabBarOverlineSize, 0.0f, 3.0f, "%.0f");
        ImGui::DragFloat(Translator::translate("TabMinWidthBase").c_str(), &style.TabMinWidthBase, 0.5f, 1.0f, 500.0f, "%.0f");
        ImGui::DragFloat(Translator::translate("TabMinWidthShrink").c_str(), &style.TabMinWidthShrink, 0.5f, 1.0f, 500.0f, "%0.f");

        {
            bool checked = style.TabCloseButtonMinWidthSelected < 0.0f;
            if(ImGui::Checkbox(Translator::translate("ShowCloseButtonOnSelectedTab").c_str(), &checked))
                style.TabCloseButtonMinWidthSelected = (float)(checked) * (-1.f);
        }

        {
            bool checked = style.TabCloseButtonMinWidthUnselected < 0.0f;
            if(ImGui::Checkbox(Translator::translate("ShowCloseButtonOnUnselectedTab").c_str(), &checked))
                style.TabCloseButtonMinWidthUnselected = (float)(checked) * (-1.f); 
        }
    }

    ImGui::SeparatorText(Translator::translate("Tables").c_str());
    {
        ImGui::SliderFloat2(Translator::translate("CellPadding").c_str(), (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2(Translator::translate("TableAngledHeadersTextAlign").c_str(), (float*)&style.TableAngledHeadersTextAlign, 0.0f, 1.0f, "%.2f");
        ImGui::SliderAngle("TableAngledHeadersAngle", &style.TableAngledHeadersAngle, -50.0f, +50.0f);
    }

    ImGui::SeparatorText(Translator::translate("Trees").c_str());
    {
        if (ImGui::BeginCombo(Translator::translate("TreeLinesFlags").c_str(), GetTreeLinesFlagsName(style.TreeLinesFlags)))
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
        ImGui::SliderFloat(Translator::translate("TreeLinesSize").c_str(), &style.TreeLinesSize, 0.0f, 2.0f, "%.0f");
        ImGui::SliderFloat(Translator::translate("TreeLinesRounding").c_str(), &style.TreeLinesRounding, 0.0f, 12.0f, "%.0f");
    }

    ImGui::SeparatorText(Translator::translate("Docking").c_str());
    {
        ImGui::SliderFloat(Translator::translate("DockingSeparatorSize").c_str(), &style.DockingSeparatorSize, 0.0f, 12.0f, "%.0f");
    }
}

void Style::draw_color_settings()
{
    auto& style = ImGui::GetStyle();

    if (ImGui::ShowStyleSelector("Colors##Selector")){}

    static ImGuiTextFilter filter;
    filter.Draw(Translator::translate("Filter").c_str(), ImGui::GetFontSize() * 16);

    static ImGuiColorEditFlags alpha_flags = 0;
    if (ImGui::RadioButton(Translator::translate("Opaque").c_str(), alpha_flags == ImGuiColorEditFlags_AlphaOpaque))
        alpha_flags = ImGuiColorEditFlags_AlphaOpaque;
    ImGui::SameLine();
    
    if (ImGui::RadioButton(Translator::translate("Alpha").c_str(), alpha_flags == ImGuiColorEditFlags_None))
        alpha_flags = ImGuiColorEditFlags_None;
    ImGui::SameLine();
    
    if (ImGui::RadioButton(Translator::translate("Both").c_str(), alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf))
        alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf;

    ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::BeginChild("##colors", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
    ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
    for (int i = 0; i < ImGuiCol_COUNT; i++)
    {
        std::string name = get_style_color_name(i);
        
        if (!filter.PassFilter(name.c_str()))
            continue;
        ImGui::PushID(i);
        
        ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
        
        // apply / revert functionality...
        if (memcmp(&style.Colors[i], &m_ReferenceStyle.Colors[i], sizeof(ImVec4)) != 0)
        {
            ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); 
            
            if(ImGui::Button(Translator::translate("Save").c_str())) 
                m_ReferenceStyle.Colors[i] = style.Colors[i];
            
            ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); 
            
            if (ImGui::Button(Translator::translate("Revert").c_str())) 
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

    if(ImGui::Button(Translator::translate("Browse").c_str()))
    {
        auto application = Frenchie::Application::Application::instance();

        application->push<FileSystem::Dialogs::ExplorerDialog>(
            Translator::translate("Select path where to search for fonts...").c_str(),
            [this, application]()
            {
                auto dialog = application->find<FileSystem::Dialogs::ExplorerDialog>();

                if(dialog == nullptr) 
                    return;

                auto path = dialog->get_path();
                application->find_or_push<CommandsQueue>()->push<CallbackCommand>(
                    [this, path, application](){application->find_or_push<Configuration::Fonts>()->scan_fonts(path);});
            }
        );
    }

    // font size
    if (ImGui::DragFloat(Translator::translate("Size").c_str(), &ImGui::GetStyle().FontSizeBase, 0.20f, 5.0f, 100.0f, "%.0f"))
        ImGui::GetStyle()._NextFrameFontSizeBase = ImGui::GetStyle().FontSizeBase;
}

void Style::draw_rendering_settings()
{
    auto& style = ImGui::GetStyle();

    ImGui::Checkbox(Translator::translate("Antialiased lines").c_str(), &style.AntiAliasedLines);
    ImGui::Checkbox(Translator::translate("Antialiased lines use texture").c_str(), &style.AntiAliasedLinesUseTex);
    ImGui::Checkbox(Translator::translate("Antialiased lines fill").c_str(), &style.AntiAliasedFill);
    ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
    ImGui::DragFloat(Translator::translate("Curve tessellation tolerance").c_str(), &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
    if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;

    // When editing the "Circle Segment Max Error" value, draw a preview of its effect on auto-tessellated circles.
    ImGui::DragFloat(Translator::translate("Circle tesseletion maimum error").c_str(), &style.CircleTessellationMaxError , 0.005f, 0.10f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    const bool show_samples = ImGui::IsItemActive();
    if (show_samples)
        ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());

    if (show_samples && ImGui::BeginTooltip())
    {
        ImGui::TextUnformatted(Translator::translate("R = radius, N = approx number of segments").c_str());
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

    ImGui::DragFloat(Translator::translate("Global alpha").c_str(), &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
    ImGui::DragFloat(Translator::translate("Disabled alpha").c_str(), &style.DisabledAlpha, 0.005f, 0.0f, 1.0f, "%.2f"); 
    ImGui::PopItemWidth();
}

std::string Style::get_style_color_name(ImGuiCol idx)
{
    // Create switch-case from enum with regexp: ImGuiCol_{.*}, --> case ImGuiCol_\1: return "\1";
    switch (idx)
    {
    case ImGuiCol_Text: return Translator::translate("Text");
    case ImGuiCol_TextDisabled: return Translator::translate("TextDisabled");
    case ImGuiCol_WindowBg: return Translator::translate("WindowBg");
    case ImGuiCol_ChildBg: return Translator::translate("ChildBg");
    case ImGuiCol_PopupBg: return Translator::translate("PopupBg");
    case ImGuiCol_Border: return Translator::translate("Border");
    case ImGuiCol_BorderShadow: return Translator::translate("BorderShadow");
    case ImGuiCol_FrameBg: return Translator::translate("FrameBg");
    case ImGuiCol_FrameBgHovered: return Translator::translate("FrameBgHovered");
    case ImGuiCol_FrameBgActive: return Translator::translate("FrameBgActive");
    case ImGuiCol_TitleBg: return Translator::translate("TitleBg");
    case ImGuiCol_TitleBgActive: return Translator::translate("TitleBgActive");
    case ImGuiCol_TitleBgCollapsed: return Translator::translate("TitleBgCollapsed");
    case ImGuiCol_MenuBarBg: return Translator::translate("MenuBarBg");
    case ImGuiCol_ScrollbarBg: return Translator::translate("ScrollbarBg");
    case ImGuiCol_ScrollbarGrab: return Translator::translate("ScrollbarGrab");
    case ImGuiCol_ScrollbarGrabHovered: return Translator::translate("ScrollbarGrabHovered");
    case ImGuiCol_ScrollbarGrabActive: return Translator::translate("ScrollbarGrabActive");
    case ImGuiCol_CheckMark: return Translator::translate("CheckMark");
    case ImGuiCol_SliderGrab: return Translator::translate("SliderGrab");
    case ImGuiCol_SliderGrabActive: return Translator::translate("SliderGrabActive");
    case ImGuiCol_Button: return Translator::translate("Button");
    case ImGuiCol_ButtonHovered: return Translator::translate("ButtonHovered");
    case ImGuiCol_ButtonActive: return Translator::translate("ButtonActive");
    case ImGuiCol_Header: return Translator::translate("Header");
    case ImGuiCol_HeaderHovered: return Translator::translate("HeaderHovered");
    case ImGuiCol_HeaderActive: return Translator::translate("HeaderActive");
    case ImGuiCol_Separator: return Translator::translate("Separator");
    case ImGuiCol_SeparatorHovered: return Translator::translate("SeparatorHovered");
    case ImGuiCol_SeparatorActive: return Translator::translate("SeparatorActive");
    case ImGuiCol_ResizeGrip: return Translator::translate("ResizeGrip");
    case ImGuiCol_ResizeGripHovered: return Translator::translate("ResizeGripHovered");
    case ImGuiCol_ResizeGripActive: return Translator::translate("ResizeGripActive");
    case ImGuiCol_InputTextCursor: return Translator::translate("InputTextCursor");
    case ImGuiCol_TabHovered: return Translator::translate("TabHovered");
    case ImGuiCol_Tab: return Translator::translate("Tab");
    case ImGuiCol_TabSelected: return Translator::translate("TabSelected");
    case ImGuiCol_TabSelectedOverline: return Translator::translate("TabSelectedOverline");
    case ImGuiCol_TabDimmed: return Translator::translate("TabDimmed");
    case ImGuiCol_TabDimmedSelected: return Translator::translate("TabDimmedSelected");
    case ImGuiCol_TabDimmedSelectedOverline: return Translator::translate("TabDimmedSelectedOverline");
    case ImGuiCol_DockingPreview: return Translator::translate("DockingPreview");
    case ImGuiCol_DockingEmptyBg: return Translator::translate("DockingEmptyBg");
    case ImGuiCol_PlotLines: return Translator::translate("PlotLines");
    case ImGuiCol_PlotLinesHovered: return Translator::translate("PlotLinesHovered");
    case ImGuiCol_PlotHistogram: return Translator::translate("PlotHistogram");
    case ImGuiCol_PlotHistogramHovered: return Translator::translate("PlotHistogramHovered");
    case ImGuiCol_TableHeaderBg: return Translator::translate("TableHeaderBg");
    case ImGuiCol_TableBorderStrong: return Translator::translate("TableBorderStrong");
    case ImGuiCol_TableBorderLight: return Translator::translate("TableBorderLight");
    case ImGuiCol_TableRowBg: return Translator::translate("TableRowBg");
    case ImGuiCol_TableRowBgAlt: return Translator::translate("TableRowBgAlt");
    case ImGuiCol_TextLink: return Translator::translate("TextLink");
    case ImGuiCol_TextSelectedBg: return Translator::translate("TextSelectedBg");
    case ImGuiCol_TreeLines: return Translator::translate("TreeLines");
    case ImGuiCol_DragDropTarget: return Translator::translate("DragDropTarget");
    case ImGuiCol_NavCursor: return Translator::translate("NavCursor");
    case ImGuiCol_NavWindowingHighlight: return Translator::translate("NavWindowingHighlight");
    case ImGuiCol_NavWindowingDimBg: return Translator::translate("NavWindowingDimBg");
    case ImGuiCol_ModalWindowDimBg: return Translator::translate("ModalWindowDimBg");
    }

    return Translator::translate("Unknown");
}