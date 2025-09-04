#include <FrenchieApplicationEditorPreferencesStyleSettingsLayer.hpp>

#include <FrenchieApplication.hpp>
#include <FrenchieApplicationEditorCommandsQueueLayer.hpp>
#include <FrenchieApplicationEditorFileSystemDialogLayer.hpp>

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

StyleSettings::StyleSettings() : 
    Frenchie::Application::Layer::Registry<StyleSettings>(STRINGIFY(StyleSettings)){}

StyleSettings::~StyleSettings(){}

std::string StyleSettings::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::Preferences), STRINGIFY(StyleSettings));
}

bool StyleSettings::awake()
{
    // load style from imgui
    m_ReferenceStyle = ImGui::GetStyle();

    return Layer::awake();
}

void StyleSettings::frame_update() 
{
    ImGui::BeginChild(get_name().c_str());
    {
        draw_style_editor();
    }

    ImGui::EndChild();
}

bool StyleSettings::serialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    // Main
    auto geometry = _Parent.append_node("Geometry");
    {
        // WindowPadding
        {
            auto vector = geometry.append_node("WindowPadding");
            vector.append_node("x").set_value_as<float>(ImGui::GetStyle().WindowPadding.x);
            vector.append_node("y").set_value_as<float>(ImGui::GetStyle().WindowPadding.y);
        }

        // FramePadding
        {
            auto vector = geometry.append_node("FramePadding");
            vector.append_node("x").set_value_as<float>(ImGui::GetStyle().FramePadding.x);
            vector.append_node("y").set_value_as<float>(ImGui::GetStyle().FramePadding.y);
        }

        // ItemSpacing
        {
            auto vector = geometry.append_node("ItemSpacing");
            vector.append_node("x").set_value_as<float>(ImGui::GetStyle().ItemSpacing.x);
            vector.append_node("y").set_value_as<float>(ImGui::GetStyle().ItemSpacing.y);
        }

        // ItemInnerSpacing
        {
            auto vector = geometry.append_node("ItemInnerSpacing");
            vector.append_node("x").set_value_as<float>(ImGui::GetStyle().ItemInnerSpacing.x);
            vector.append_node("y").set_value_as<float>(ImGui::GetStyle().ItemInnerSpacing.y);
        }

        // TouchExtraPadding
        {
            auto vector = geometry.append_node("TouchExtraPadding");
            vector.append_node("x").set_value_as<float>(ImGui::GetStyle().TouchExtraPadding.x);
            vector.append_node("y").set_value_as<float>(ImGui::GetStyle().TouchExtraPadding.y);
        }

        geometry.append_node("IndentSpacing").set_value_as<float>(ImGui::GetStyle().IndentSpacing);
        geometry.append_node("ScrollbarSize").set_value_as<float>(ImGui::GetStyle().ScrollbarSize);
        geometry.append_node("GrabMinSize").set_value_as<float>(ImGui::GetStyle().GrabMinSize);
    }

    // Borders
    {
        geometry.append_node("WindowBorderSize").set_value_as<float>(ImGui::GetStyle().WindowBorderSize);
        geometry.append_node("ChildBorderSize").set_value_as<float>(ImGui::GetStyle().ChildBorderSize);
        geometry.append_node("PopupBorderSize").set_value_as<float>(ImGui::GetStyle().PopupBorderSize);
        geometry.append_node("FrameBorderSize").set_value_as<float>(ImGui::GetStyle().FrameBorderSize);
        geometry.append_node("TabBorderSize").set_value_as<float>(ImGui::GetStyle().TabBorderSize);
        geometry.append_node("TabBarBorderSize").set_value_as<float>(ImGui::GetStyle().TabBarBorderSize);
    }

    // Rounding
    {
        geometry.append_node("WindowRounding").set_value_as<float>(ImGui::GetStyle().WindowRounding);
        geometry.append_node("ChildRounding").set_value_as<float>(ImGui::GetStyle().ChildRounding);
        geometry.append_node("FrameRounding").set_value_as<float>(ImGui::GetStyle().FrameRounding);
        geometry.append_node("PopupRounding").set_value_as<float>(ImGui::GetStyle().PopupRounding);
        geometry.append_node("ScrollbarRounding").set_value_as<float>(ImGui::GetStyle().ScrollbarRounding);
        geometry.append_node("GrabRounding").set_value_as<float>(ImGui::GetStyle().GrabRounding);
        geometry.append_node("TabRounding").set_value_as<float>(ImGui::GetStyle().TabRounding);
    }

    // Tabs
    {
        geometry.append_node("TabBarOverlineSize").set_value_as<float>(ImGui::GetStyle().TabBarOverlineSize);
        geometry.append_node("TabMinWidthBase").set_value_as<float>(ImGui::GetStyle().TabMinWidthBase);
        geometry.append_node("TabMinWidthShrink").set_value_as<float>(ImGui::GetStyle().TabMinWidthShrink);
        geometry.append_node("TabCloseButtonMinWidthSelected").set_value_as<float>(ImGui::GetStyle().TabCloseButtonMinWidthSelected);
        geometry.append_node("TabCloseButtonMinWidthUnselected").set_value_as<float>(ImGui::GetStyle().TabCloseButtonMinWidthUnselected);
    }

    // Tables
    {
        {
            auto vector = geometry.append_node("CellPadding");
            vector.append_node("x").set_value_as<float>(ImGui::GetStyle().CellPadding.x);
            vector.append_node("y").set_value_as<float>(ImGui::GetStyle().CellPadding.y);
        }

        {
            auto vector = geometry.append_node("TableAngledHeadersTextAlign");
            vector.append_node("x").set_value_as<float>(ImGui::GetStyle().CellPadding.x);
            vector.append_node("y").set_value_as<float>(ImGui::GetStyle().CellPadding.y);
        }

        geometry.append_node("TableAngledHeadersAngle").set_value_as<float>(ImGui::GetStyle().TableAngledHeadersAngle);
    }

    // Trees
    {
        geometry.append_node("TreeLinesFlags").set_value_as<size_t>(ImGui::GetStyle().TreeLinesFlags);
        geometry.append_node("TreeLinesSize").set_value_as<float>(ImGui::GetStyle().TreeLinesSize);
        geometry.append_node("TreeLinesRounding").set_value_as<float>(ImGui::GetStyle().TreeLinesRounding);
    }

    // Docking
    {
        geometry.append_node("DockingSeparatorSize").set_value_as<float>(ImGui::GetStyle().DockingSeparatorSize);
    }

    // Colors
    auto colors = _Parent.append_node("Colors");
    {
        for (int i = 0; i < ImGuiCol_::ImGuiCol_COUNT; i++)
        {
            auto color = colors.append_node(ImGui::GetStyleColorName(i));
            color.append_node("R").set_value_as<int>((int)(ImGui::GetStyle().Colors[i].x * 255.f));
            color.append_node("G").set_value_as<int>((int)(ImGui::GetStyle().Colors[i].y * 255.f));
            color.append_node("B").set_value_as<int>((int)(ImGui::GetStyle().Colors[i].z * 255.f));
            color.append_node("A").set_value_as<int>((int)(ImGui::GetStyle().Colors[i].w * 255.f));
        }
    }

    // Fonts
    auto fonts = _Parent.append_node("Fonts");
    {
        fonts.append_node("FontsLoadPath", Frenchie::Core::Helpers::String::as_utf8(m_FontsLoadPath.wstring()).c_str());
        fonts.append_node("FontSizeBase").set_value_as<float>(ImGui::GetStyle().FontSizeBase);
        fonts.append_node("Font", ImGui::GetFont()->GetDebugName());
    }

    return true;
}

bool StyleSettings::deserialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    // parse geometry
    auto geometry = _Parent.find_node("Geometry");

    if(geometry.is_valid())
    {
        // Main
        {
            // WindowPadding
            auto WindowPadding = geometry.find_node("WindowPadding");

            if(WindowPadding.find_node("x").is_valid() && 
                WindowPadding.find_node("y").is_valid())
            {
                ImGui::GetStyle().WindowPadding = 
                    ImVec2(WindowPadding.find_node("x").get_value_as<float>(), WindowPadding.find_node("y").get_value_as<float>());
            }

            // FramePadding
            auto FramePadding = geometry.find_node("FramePadding");

            if(FramePadding.find_node("x").is_valid() && 
                FramePadding.find_node("y").is_valid())
            {
                ImGui::GetStyle().FramePadding = 
                    ImVec2(FramePadding.find_node("x").get_value_as<float>(), FramePadding.find_node("y").get_value_as<float>());
            }

            // ItemSpacing
            auto ItemSpacing = geometry.find_node("ItemSpacing");

            if(ItemSpacing.find_node("x").is_valid() && 
                ItemSpacing.find_node("y").is_valid())
            {
                ImGui::GetStyle().ItemSpacing = 
                    ImVec2(ItemSpacing.find_node("x").get_value_as<float>(), ItemSpacing.find_node("y").get_value_as<float>());
            }    

            // ItemInnerSpacing
            auto ItemInnerSpacing  = geometry.find_node("ItemInnerSpacing");

            if(ItemInnerSpacing.find_node("x").is_valid() && 
                ItemInnerSpacing.find_node("y").is_valid())
            {
                ImGui::GetStyle().ItemInnerSpacing = 
                    ImVec2(ItemInnerSpacing.find_node("x").get_value_as<float>(), ItemInnerSpacing.find_node("y").get_value_as<float>());
            } 

            // TouchExtraPadding
            auto TouchExtraPadding = geometry.find_node("TouchExtraPadding");

            if(TouchExtraPadding.find_node("x").is_valid() && 
                TouchExtraPadding.find_node("y").is_valid())
            {
                ImGui::GetStyle().TouchExtraPadding = 
                    ImVec2(TouchExtraPadding.find_node("x").get_value_as<float>(), TouchExtraPadding.find_node("y").get_value_as<float>());
            } 

            // IndentSpacing
            auto IndentSpacing = geometry.find_node("IndentSpacing");

            if(IndentSpacing.is_valid()) 
                ImGui::GetStyle().IndentSpacing = IndentSpacing.get_value_as<float>();

            // ScrollbarSize
            auto ScrollbarSize = geometry.find_node("ScrollbarSize");

            if(ScrollbarSize.is_valid()) 
                ImGui::GetStyle().ScrollbarSize = ScrollbarSize.get_value_as<float>();
            
            // GrabMinSize
            auto GrabMinSize = geometry.find_node("GrabMinSize");

            if(GrabMinSize.is_valid()) 
                ImGui::GetStyle().GrabMinSize = GrabMinSize.get_value_as<float>();
        }

        // Borders
        {
            auto WindowBorderSize = geometry.find_node("WindowBorderSize");
            if(WindowBorderSize.is_valid()) 
                ImGui::GetStyle().WindowBorderSize = WindowBorderSize.get_value_as<float>();

            auto ChildBorderSize = geometry.find_node("ChildBorderSize");
            if(ChildBorderSize.is_valid()) 
                ImGui::GetStyle().ChildBorderSize = ChildBorderSize.get_value_as<float>();

            auto PopupBorderSize = geometry.find_node("PopupBorderSize");
            if(PopupBorderSize.is_valid()) 
                ImGui::GetStyle().PopupBorderSize = PopupBorderSize.get_value_as<float>();

            auto FrameBorderSize = geometry.find_node("FrameBorderSize");
            if(FrameBorderSize.is_valid()) 
                ImGui::GetStyle().FrameBorderSize = FrameBorderSize.get_value_as<float>();

            auto TabBorderSize = geometry.find_node("TabBorderSize");
            if(TabBorderSize.is_valid()) 
                ImGui::GetStyle().TabBorderSize = TabBorderSize.get_value_as<float>();

            auto TabBarBorderSize = geometry.find_node("TabBarBorderSize");
            if(TabBarBorderSize.is_valid()) 
                ImGui::GetStyle().TabBarBorderSize = TabBarBorderSize.get_value_as<float>();
        }

        // Rounding
        {
            auto WindowRounding = geometry.find_node("WindowRounding");
            if(WindowRounding.is_valid()) 
                ImGui::GetStyle().WindowRounding = WindowRounding.get_value_as<float>();        

            auto ChildRounding = geometry.find_node("ChildRounding");
            if(ChildRounding.is_valid()) 
                ImGui::GetStyle().ChildRounding = ChildRounding.get_value_as<float>();

            auto FrameRounding = geometry.find_node("FrameRounding");
            if(FrameRounding.is_valid()) 
                ImGui::GetStyle().FrameRounding = FrameRounding.get_value_as<float>();

            auto PopupRounding = geometry.find_node("PopupRounding");
            if(PopupRounding.is_valid()) 
                ImGui::GetStyle().PopupRounding = PopupRounding.get_value_as<float>();

            auto ScrollbarRounding = geometry.find_node("ScrollbarRounding");
            if(ScrollbarRounding.is_valid()) 
                ImGui::GetStyle().ScrollbarRounding = ScrollbarRounding.get_value_as<float>();

            auto GrabRounding = geometry.find_node("GrabRounding");
            if(GrabRounding.is_valid()) 
                ImGui::GetStyle().GrabRounding = GrabRounding.get_value_as<float>();

            auto TabRounding = geometry.find_node("TabRounding");
            if(TabRounding.is_valid()) 
                ImGui::GetStyle().TabRounding = TabRounding.get_value_as<float>();
        }

        // Tabs
        {
            auto TabBarOverlineSize = geometry.find_node("TabBarOverlineSize");
            if(TabBarOverlineSize.is_valid()) 
                ImGui::GetStyle().TabBarOverlineSize = TabBarOverlineSize.get_value_as<float>();

            auto TabMinWidthBase = geometry.find_node("TabMinWidthBase");
            if(TabMinWidthBase.is_valid()) 
                ImGui::GetStyle().TabMinWidthBase = TabMinWidthBase.get_value_as<float>();

            auto TabMinWidthShrink = geometry.find_node("TabMinWidthShrink");
            if(TabMinWidthShrink.is_valid()) 
                ImGui::GetStyle().TabMinWidthShrink = TabMinWidthShrink.get_value_as<float>();

            auto TabCloseButtonMinWidthSelected = geometry.find_node("TabCloseButtonMinWidthSelected");
            if(TabCloseButtonMinWidthSelected.is_valid()) 
                ImGui::GetStyle().TabCloseButtonMinWidthSelected = TabCloseButtonMinWidthSelected.get_value_as<float>();

            auto TabCloseButtonMinWidthUnselected = geometry.find_node("TabCloseButtonMinWidthUnselected");
            if(TabCloseButtonMinWidthUnselected.is_valid()) 
                ImGui::GetStyle().TabCloseButtonMinWidthUnselected = TabCloseButtonMinWidthUnselected.get_value_as<float>();
        }

        // Tables
        {
            // CellPadding
            auto CellPadding = geometry.find_node("CellPadding");

            if(CellPadding.find_node("x").is_valid() && 
                CellPadding.find_node("y").is_valid())
            {
                ImGui::GetStyle().CellPadding = 
                    ImVec2(CellPadding.find_node("x").get_value_as<float>(), CellPadding.find_node("y").get_value_as<float>());
            }

            // TableAngledHeadersTextAlign
            auto TableAngledHeadersTextAlign = geometry.find_node("TableAngledHeadersTextAlign");

            if(TableAngledHeadersTextAlign.find_node("x").is_valid() && 
                TableAngledHeadersTextAlign.find_node("y").is_valid())
            {
                ImGui::GetStyle().TableAngledHeadersTextAlign = 
                    ImVec2(TableAngledHeadersTextAlign.find_node("x").get_value_as<float>(), TableAngledHeadersTextAlign.find_node("y").get_value_as<float>());
            }

            auto TableAngledHeadersAngle = geometry.find_node("TableAngledHeadersAngle");
            if(TableAngledHeadersAngle.is_valid()) 
                ImGui::GetStyle().TableAngledHeadersAngle = TableAngledHeadersAngle.get_value_as<float>();
        }

        // Trees
        {
            auto TreeLinesFlags = geometry.find_node("TreeLinesFlags");
            if(TreeLinesFlags.is_valid()) 
                ImGui::GetStyle().TreeLinesFlags = TreeLinesFlags.get_value_as<ImGuiTreeNodeFlags>();

            auto TreeLinesSize = geometry.find_node("TreeLinesSize");
            if(TreeLinesSize.is_valid()) 
                ImGui::GetStyle().TreeLinesSize = TreeLinesSize.get_value_as<float>();

            auto TreeLinesRounding = geometry.find_node("TreeLinesRounding");
            if(TreeLinesRounding.is_valid()) 
                ImGui::GetStyle().TreeLinesRounding = TreeLinesRounding.get_value_as<float>();
        }

        // Docking
        {
            auto DockingSeparatorSize = geometry.find_node("DockingSeparatorSize");
            if(DockingSeparatorSize.is_valid()) 
                ImGui::GetStyle().DockingSeparatorSize = DockingSeparatorSize.get_value_as<float>();
        }
    }

    // parse colors
    auto colors = _Parent.find_node("Colors");

    if(colors.is_valid())
    {
        for (int i = 0; i < ImGuiCol_::ImGuiCol_COUNT; i++)
        {
            auto color = colors.find_node(ImGui::GetStyleColorName(i));

            if(!color.is_valid()) 
                continue;

            auto r = color.find_node("R");
            auto g = color.find_node("G");
            auto b = color.find_node("B");
            auto a = color.find_node("A");

            if(r.is_valid()) 
                ImGui::GetStyle().Colors[i].x = r.get_value_as<float>() / 255.f;

            if(g.is_valid()) 
                ImGui::GetStyle().Colors[i].y = g.get_value_as<float>() / 255.f;

            if(b.is_valid()) 
                ImGui::GetStyle().Colors[i].z = b.get_value_as<float>() / 255.f;

            if(a.is_valid()) 
                ImGui::GetStyle().Colors[i].w = a.get_value_as<float>() / 255.f;
        }
    }

    // parse fonts
    auto fonts = _Parent.find_node("Fonts");
    {
        if(fonts.find_node("FontSizeBase").is_valid())
            ImGui::GetStyle().FontSizeBase = fonts.find_node("FontSizeBase").get_value_as<float>();

        if(fonts.find_node("FontsLoadPath").is_valid())
        {
            m_FontsLoadPath = std::filesystem::path(fonts.find_node("FontsLoadPath").get_value());

            // load fonts
            load_fonts(m_FontsLoadPath);
        }

        // setup font
        if(fonts.find_node("Font").is_valid())
        {
            auto fontName = std::string(fonts.find_node("Font").get_value());

            ImGuiIO& io = ImGui::GetIO();
            ImFont* font_current = ImGui::GetFont();

            for (ImFont* font : io.Fonts->Fonts)
            {
                if (std::string(font->GetDebugName()) == fontName)
                {
                    io.FontDefault = font;
                    break;
                }
            }   
        }
    }

    return true;
}

void StyleSettings::draw_style_editor()
{
    //ImGuiStyle& style = ImGui::GetStyle();

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Geometry"))
        {
            draw_geometry_settings();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Colors"))
        {
            draw_color_settings();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Fonts"))
        {
            draw_fonts_settings();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Rendering"))
        {
            draw_rendering_settings();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void StyleSettings::draw_geometry_settings()
{
    auto& style = ImGui::GetStyle();

    ImGui::SeparatorText("Main");
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

    ImGui::SeparatorText("Borders");
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

    ImGui::SeparatorText("Rounding");
    {
        ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
    }

    ImGui::SeparatorText("Tabs");
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

    ImGui::SeparatorText("Tables");
    {
        ImGui::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2("TableAngledHeadersTextAlign", (float*)&style.TableAngledHeadersTextAlign, 0.0f, 1.0f, "%.2f");
        ImGui::SliderAngle("TableAngledHeadersAngle", &style.TableAngledHeadersAngle, -50.0f, +50.0f);
    }

    ImGui::SeparatorText("Trees");
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

    ImGui::SeparatorText("Docking");
    {
        ImGui::SliderFloat("DockingSeparatorSize", &style.DockingSeparatorSize, 0.0f, 12.0f, "%.0f");
    }
}

void StyleSettings::draw_color_settings()
{
    auto& style = ImGui::GetStyle();

    if (ImGui::ShowStyleSelector("Colors##Selector")){}

    static ImGuiTextFilter filter;
    filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

    static ImGuiColorEditFlags alpha_flags = 0;
    if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_AlphaOpaque))
        alpha_flags = ImGuiColorEditFlags_AlphaOpaque;
    ImGui::SameLine();
    
    if (ImGui::RadioButton("Alpha", alpha_flags == ImGuiColorEditFlags_None))
        alpha_flags = ImGuiColorEditFlags_None;
    ImGui::SameLine();
    
    if (ImGui::RadioButton("Both", alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf))
        alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf;

    ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::BeginChild("##colors", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
    ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
    for (int i = 0; i < ImGuiCol_COUNT; i++)
    {
        const char* name = ImGui::GetStyleColorName(i);
        
        if (!filter.PassFilter(name))
            continue;
        ImGui::PushID(i);
        
        ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
        
        // apply / revert functionality...
        if (memcmp(&style.Colors[i], &m_ReferenceStyle.Colors[i], sizeof(ImVec4)) != 0)
        {
            ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); 
            
            if(ImGui::Button("Save")) 
                m_ReferenceStyle.Colors[i] = style.Colors[i];
            
            ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); 
            
            if (ImGui::Button("Revert")) 
                style.Colors[i] = m_ReferenceStyle.Colors[i];
        }

        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        ImGui::TextUnformatted(name);
        ImGui::PopID();
    }
    ImGui::PopItemWidth();
    ImGui::EndChild();
}

void StyleSettings::draw_fonts_settings()
{
    auto& style = ImGui::GetStyle();

    // font loader
    ImGui::TextUnformatted(m_FontsLoadPath.string().c_str());
    ImGui::SameLine();
    if(ImGui::Button("Browse"))
    {
        Frenchie::Application::Application::instance()->push<FilesOpenDialog>(
            [this]()
            {
                auto dialog =  Frenchie::Application::Application::instance()->find<FilesOpenDialog>();

                if(dialog != nullptr)
                {
                    auto path = dialog->get_path();

                    Frenchie::Application::Application::instance()->find_or_push<CommandsQueue>()->push<CallbackCommand>(
                        [this, path]()
                        {
                            load_fonts(path);
                        }
                    );
                }
            },
            "Select directory where to look for the fonts .ttf files..."
        );
    }

    // font selector
    ImGui::ShowFontSelector("Fonts##Selector");
    
    // font size
    if (ImGui::DragFloat("FontSizeBase", &style.FontSizeBase, 0.20f, 5.0f, 100.0f, "%.0f"))
        style._NextFrameFontSizeBase = style.FontSizeBase;
}

void StyleSettings::draw_rendering_settings()
{
    auto& style = ImGui::GetStyle();

    ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
    ImGui::SameLine();

    ImGui::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
    ImGui::SameLine();

    ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
    ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
    ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
    if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;

    // When editing the "Circle Segment Max Error" value, draw a preview of its effect on auto-tessellated circles.
    ImGui::DragFloat("Circle Tessellation Max Error", &style.CircleTessellationMaxError , 0.005f, 0.10f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
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
    ImGui::SameLine();

    ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
    ImGui::DragFloat("Disabled Alpha", &style.DisabledAlpha, 0.005f, 0.0f, 1.0f, "%.2f"); 
    ImGui::SameLine();
    ImGui::PopItemWidth();
}

void StyleSettings::load_fonts(const std::filesystem::path& _Path)
{
    if(!std::filesystem::exists(_Path))
        return;

    // retrive ImGui IO
    auto& io = ImGui::GetIO();
    io.Fonts->Clear();

    // recursivelly scan path for .ttf fonts
    for(const auto& directory :
         std::filesystem::recursive_directory_iterator(_Path, std::filesystem::directory_options::skip_permission_denied))
    {
        if(directory.is_directory() ||
            directory.path().extension() != ".ttf")
            continue;

        io.Fonts->AddFontFromFileTTF(
            pugi::as_utf8(directory.path().wstring()).c_str(),
            ImGui::GetStyle().FontSizeBase,
            nullptr,
            io.Fonts->GetGlyphRangesCyrillic());
    }

    // build fonts
    io.Fonts->Build();

    // reload app
    Frenchie::Application::Application::instance()->reload();

    // setup fonts load path
    m_FontsLoadPath = _Path;
}