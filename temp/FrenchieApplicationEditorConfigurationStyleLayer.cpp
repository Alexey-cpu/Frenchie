#include <FrenchieApplicationEditorConfigurationStyleLayer.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

// Editor
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>
#include <FrenchieEditorConfigurationTranslatorLayer.hpp>

// Core
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreThreadPool.hpp>

// SPDLOG
#include "spdlog/sinks/basic_file_sink.h"

// STL
#include <chrono>
#include <iostream>
#include <fstream>
#include <functional>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Editor::FileSystem;
using namespace Frenchie::Editor::Configuration;

Style::Style() : 
    Layer(Translator::translate(STRINGIFY(Style))){}
Style::~Style(){}

// Frenchie::Application::Layer
bool Style::allows_multiple_instances() const
{
    return false;
}

// Frenchie::Core::Serialization::ISerializer
bool Style::serialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto style = _Parent.append_node("Style");

    // WindowPadding
    {
        auto vector = style.append_node("WindowPadding");
        vector.append_node("x").set_value_as<float>(ImGui::GetStyle().WindowPadding.x);
        vector.append_node("y").set_value_as<float>(ImGui::GetStyle().WindowPadding.y);
    }

    // FramePadding
    {
        auto vector = style.append_node("FramePadding");
        vector.append_node("x").set_value_as<float>(ImGui::GetStyle().FramePadding.x);
        vector.append_node("y").set_value_as<float>(ImGui::GetStyle().FramePadding.y);
    }

    // ItemSpacing
    {
        auto vector = style.append_node("ItemSpacing");
        vector.append_node("x").set_value_as<float>(ImGui::GetStyle().ItemSpacing.x);
        vector.append_node("y").set_value_as<float>(ImGui::GetStyle().ItemSpacing.y);
    }

    // ItemInnerSpacing
    {
        auto vector = style.append_node("ItemInnerSpacing");
        vector.append_node("x").set_value_as<float>(ImGui::GetStyle().ItemInnerSpacing.x);
        vector.append_node("y").set_value_as<float>(ImGui::GetStyle().ItemInnerSpacing.y);
    }

    // TouchExtraPadding
    {
        auto vector = style.append_node("TouchExtraPadding");
        vector.append_node("x").set_value_as<float>(ImGui::GetStyle().TouchExtraPadding.x);
        vector.append_node("y").set_value_as<float>(ImGui::GetStyle().TouchExtraPadding.y);
    }

    style.append_node("IndentSpacing").set_value_as<float>(ImGui::GetStyle().IndentSpacing);
    style.append_node("ScrollbarSize").set_value_as<float>(ImGui::GetStyle().ScrollbarSize);
    style.append_node("GrabMinSize").set_value_as<float>(ImGui::GetStyle().GrabMinSize);

    // Borders
    {
        style.append_node("WindowBorderSize").set_value_as<float>(ImGui::GetStyle().WindowBorderSize);
        style.append_node("ChildBorderSize").set_value_as<float>(ImGui::GetStyle().ChildBorderSize);
        style.append_node("PopupBorderSize").set_value_as<float>(ImGui::GetStyle().PopupBorderSize);
        style.append_node("FrameBorderSize").set_value_as<float>(ImGui::GetStyle().FrameBorderSize);
        style.append_node("TabBorderSize").set_value_as<float>(ImGui::GetStyle().TabBorderSize);
        style.append_node("TabBarBorderSize").set_value_as<float>(ImGui::GetStyle().TabBarBorderSize);
    }

    // Rounding
    {
        style.append_node("WindowRounding").set_value_as<float>(ImGui::GetStyle().WindowRounding);
        style.append_node("ChildRounding").set_value_as<float>(ImGui::GetStyle().ChildRounding);
        style.append_node("FrameRounding").set_value_as<float>(ImGui::GetStyle().FrameRounding);
        style.append_node("PopupRounding").set_value_as<float>(ImGui::GetStyle().PopupRounding);
        style.append_node("ScrollbarRounding").set_value_as<float>(ImGui::GetStyle().ScrollbarRounding);
        style.append_node("GrabRounding").set_value_as<float>(ImGui::GetStyle().GrabRounding);
        style.append_node("TabRounding").set_value_as<float>(ImGui::GetStyle().TabRounding);
    }

    // Tabs
    {
        style.append_node("TabBarOverlineSize").set_value_as<float>(ImGui::GetStyle().TabBarOverlineSize);
        style.append_node("TabMinWidthBase").set_value_as<float>(ImGui::GetStyle().TabMinWidthBase);
        style.append_node("TabMinWidthShrink").set_value_as<float>(ImGui::GetStyle().TabMinWidthShrink);
        style.append_node("TabCloseButtonMinWidthSelected").set_value_as<float>(ImGui::GetStyle().TabCloseButtonMinWidthSelected);
        style.append_node("TabCloseButtonMinWidthUnselected").set_value_as<float>(ImGui::GetStyle().TabCloseButtonMinWidthUnselected);
    }

    // Tables
    {
        {
            auto vector = style.append_node("CellPadding");
            vector.append_node("x").set_value_as<float>(ImGui::GetStyle().CellPadding.x);
            vector.append_node("y").set_value_as<float>(ImGui::GetStyle().CellPadding.y);
        }

        {
            auto vector = style.append_node("TableAngledHeadersTextAlign");
            vector.append_node("x").set_value_as<float>(ImGui::GetStyle().CellPadding.x);
            vector.append_node("y").set_value_as<float>(ImGui::GetStyle().CellPadding.y);
        }

        style.append_node("TableAngledHeadersAngle").set_value_as<float>(ImGui::GetStyle().TableAngledHeadersAngle);
    }

    // Trees
    {
        style.append_node("TreeLinesFlags").set_value_as<size_t>(ImGui::GetStyle().TreeLinesFlags);
        style.append_node("TreeLinesSize").set_value_as<float>(ImGui::GetStyle().TreeLinesSize);
        style.append_node("TreeLinesRounding").set_value_as<float>(ImGui::GetStyle().TreeLinesRounding);
    }

    // Docking
    {
        style.append_node("DockingSeparatorSize").set_value_as<float>(ImGui::GetStyle().DockingSeparatorSize);
    }

    // Colors
    {
        auto colors = style.append_node("Colors");

        for (int i = 0; i < ImGuiCol_::ImGuiCol_COUNT; i++)
        {
            auto color = colors.append_node(ImGui::GetStyleColorName(i));
            color.append_node("R").set_value_as<int>((int)(ImGui::GetStyle().Colors[i].x * 255.f));
            color.append_node("G").set_value_as<int>((int)(ImGui::GetStyle().Colors[i].y * 255.f));
            color.append_node("B").set_value_as<int>((int)(ImGui::GetStyle().Colors[i].z * 255.f));
            color.append_node("A").set_value_as<int>((int)(ImGui::GetStyle().Colors[i].w * 255.f));
        }
    }

    return true;
}

bool Style::deserialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    // parse geometry
    auto style = _Parent.find_node("Style");

    if(style.is_valid())
    {
        // Main
        {
            // WindowPadding
            auto WindowPadding = style.find_node("WindowPadding");

            if(WindowPadding.find_node("x").is_valid() && 
                WindowPadding.find_node("y").is_valid())
            {
                ImGui::GetStyle().WindowPadding = 
                    ImVec2(WindowPadding.find_node("x").get_value_as<float>(), WindowPadding.find_node("y").get_value_as<float>());
            }

            // FramePadding
            auto FramePadding = style.find_node("FramePadding");

            if(FramePadding.find_node("x").is_valid() && 
                FramePadding.find_node("y").is_valid())
            {
                ImGui::GetStyle().FramePadding = 
                    ImVec2(FramePadding.find_node("x").get_value_as<float>(), FramePadding.find_node("y").get_value_as<float>());
            }

            // ItemSpacing
            auto ItemSpacing = style.find_node("ItemSpacing");

            if(ItemSpacing.find_node("x").is_valid() && 
                ItemSpacing.find_node("y").is_valid())
            {
                ImGui::GetStyle().ItemSpacing = 
                    ImVec2(ItemSpacing.find_node("x").get_value_as<float>(), ItemSpacing.find_node("y").get_value_as<float>());
            }    

            // ItemInnerSpacing
            auto ItemInnerSpacing  = style.find_node("ItemInnerSpacing");

            if(ItemInnerSpacing.find_node("x").is_valid() && 
                ItemInnerSpacing.find_node("y").is_valid())
            {
                ImGui::GetStyle().ItemInnerSpacing = 
                    ImVec2(ItemInnerSpacing.find_node("x").get_value_as<float>(), ItemInnerSpacing.find_node("y").get_value_as<float>());
            } 

            // TouchExtraPadding
            auto TouchExtraPadding = style.find_node("TouchExtraPadding");

            if(TouchExtraPadding.find_node("x").is_valid() && 
                TouchExtraPadding.find_node("y").is_valid())
            {
                ImGui::GetStyle().TouchExtraPadding = 
                    ImVec2(TouchExtraPadding.find_node("x").get_value_as<float>(), TouchExtraPadding.find_node("y").get_value_as<float>());
            } 

            // IndentSpacing
            auto IndentSpacing = style.find_node("IndentSpacing");

            if(IndentSpacing.is_valid()) 
                ImGui::GetStyle().IndentSpacing = IndentSpacing.get_value_as<float>();

            // ScrollbarSize
            auto ScrollbarSize = style.find_node("ScrollbarSize");

            if(ScrollbarSize.is_valid()) 
                ImGui::GetStyle().ScrollbarSize = ScrollbarSize.get_value_as<float>();
            
            // GrabMinSize
            auto GrabMinSize = style.find_node("GrabMinSize");

            if(GrabMinSize.is_valid()) 
                ImGui::GetStyle().GrabMinSize = GrabMinSize.get_value_as<float>();
        }

        // Borders
        {
            auto WindowBorderSize = style.find_node("WindowBorderSize");
            if(WindowBorderSize.is_valid()) 
                ImGui::GetStyle().WindowBorderSize = WindowBorderSize.get_value_as<float>();

            auto ChildBorderSize = style.find_node("ChildBorderSize");
            if(ChildBorderSize.is_valid()) 
                ImGui::GetStyle().ChildBorderSize = ChildBorderSize.get_value_as<float>();

            auto PopupBorderSize = style.find_node("PopupBorderSize");
            if(PopupBorderSize.is_valid()) 
                ImGui::GetStyle().PopupBorderSize = PopupBorderSize.get_value_as<float>();

            auto FrameBorderSize = style.find_node("FrameBorderSize");
            if(FrameBorderSize.is_valid()) 
                ImGui::GetStyle().FrameBorderSize = FrameBorderSize.get_value_as<float>();

            auto TabBorderSize = style.find_node("TabBorderSize");
            if(TabBorderSize.is_valid()) 
                ImGui::GetStyle().TabBorderSize = TabBorderSize.get_value_as<float>();

            auto TabBarBorderSize = style.find_node("TabBarBorderSize");
            if(TabBarBorderSize.is_valid()) 
                ImGui::GetStyle().TabBarBorderSize = TabBarBorderSize.get_value_as<float>();
        }

        // Rounding
        {
            auto WindowRounding = style.find_node("WindowRounding");
            if(WindowRounding.is_valid()) 
                ImGui::GetStyle().WindowRounding = WindowRounding.get_value_as<float>();        

            auto ChildRounding = style.find_node("ChildRounding");
            if(ChildRounding.is_valid()) 
                ImGui::GetStyle().ChildRounding = ChildRounding.get_value_as<float>();

            auto FrameRounding = style.find_node("FrameRounding");
            if(FrameRounding.is_valid()) 
                ImGui::GetStyle().FrameRounding = FrameRounding.get_value_as<float>();

            auto PopupRounding = style.find_node("PopupRounding");
            if(PopupRounding.is_valid()) 
                ImGui::GetStyle().PopupRounding = PopupRounding.get_value_as<float>();

            auto ScrollbarRounding = style.find_node("ScrollbarRounding");
            if(ScrollbarRounding.is_valid()) 
                ImGui::GetStyle().ScrollbarRounding = ScrollbarRounding.get_value_as<float>();

            auto GrabRounding = style.find_node("GrabRounding");
            if(GrabRounding.is_valid()) 
                ImGui::GetStyle().GrabRounding = GrabRounding.get_value_as<float>();

            auto TabRounding = style.find_node("TabRounding");
            if(TabRounding.is_valid()) 
                ImGui::GetStyle().TabRounding = TabRounding.get_value_as<float>();
        }

        // Tabs
        {
            auto TabBarOverlineSize = style.find_node("TabBarOverlineSize");
            if(TabBarOverlineSize.is_valid()) 
                ImGui::GetStyle().TabBarOverlineSize = TabBarOverlineSize.get_value_as<float>();

            auto TabMinWidthBase = style.find_node("TabMinWidthBase");
            if(TabMinWidthBase.is_valid()) 
                ImGui::GetStyle().TabMinWidthBase = TabMinWidthBase.get_value_as<float>();

            auto TabMinWidthShrink = style.find_node("TabMinWidthShrink");
            if(TabMinWidthShrink.is_valid()) 
                ImGui::GetStyle().TabMinWidthShrink = TabMinWidthShrink.get_value_as<float>();

            auto TabCloseButtonMinWidthSelected = style.find_node("TabCloseButtonMinWidthSelected");
            if(TabCloseButtonMinWidthSelected.is_valid()) 
                ImGui::GetStyle().TabCloseButtonMinWidthSelected = TabCloseButtonMinWidthSelected.get_value_as<float>();

            auto TabCloseButtonMinWidthUnselected = style.find_node("TabCloseButtonMinWidthUnselected");
            if(TabCloseButtonMinWidthUnselected.is_valid()) 
                ImGui::GetStyle().TabCloseButtonMinWidthUnselected = TabCloseButtonMinWidthUnselected.get_value_as<float>();
        }

        // Tables
        {
            // CellPadding
            auto CellPadding = style.find_node("CellPadding");

            if(CellPadding.find_node("x").is_valid() && 
                CellPadding.find_node("y").is_valid())
            {
                ImGui::GetStyle().CellPadding = 
                    ImVec2(CellPadding.find_node("x").get_value_as<float>(), CellPadding.find_node("y").get_value_as<float>());
            }

            // TableAngledHeadersTextAlign
            auto TableAngledHeadersTextAlign = style.find_node("TableAngledHeadersTextAlign");

            if(TableAngledHeadersTextAlign.find_node("x").is_valid() && 
                TableAngledHeadersTextAlign.find_node("y").is_valid())
            {
                ImGui::GetStyle().TableAngledHeadersTextAlign = 
                    ImVec2(TableAngledHeadersTextAlign.find_node("x").get_value_as<float>(), TableAngledHeadersTextAlign.find_node("y").get_value_as<float>());
            }

            auto TableAngledHeadersAngle = style.find_node("TableAngledHeadersAngle");
            if(TableAngledHeadersAngle.is_valid()) 
                ImGui::GetStyle().TableAngledHeadersAngle = TableAngledHeadersAngle.get_value_as<float>();
        }

        // Trees
        {
            auto TreeLinesFlags = style.find_node("TreeLinesFlags");
            if(TreeLinesFlags.is_valid()) 
                ImGui::GetStyle().TreeLinesFlags = TreeLinesFlags.get_value_as<ImGuiTreeNodeFlags>();

            auto TreeLinesSize = style.find_node("TreeLinesSize");
            if(TreeLinesSize.is_valid()) 
                ImGui::GetStyle().TreeLinesSize = TreeLinesSize.get_value_as<float>();

            auto TreeLinesRounding = style.find_node("TreeLinesRounding");
            if(TreeLinesRounding.is_valid()) 
                ImGui::GetStyle().TreeLinesRounding = TreeLinesRounding.get_value_as<float>();
        }

        // Docking
        {
            auto DockingSeparatorSize = style.find_node("DockingSeparatorSize");
            if(DockingSeparatorSize.is_valid()) 
                ImGui::GetStyle().DockingSeparatorSize = DockingSeparatorSize.get_value_as<float>();
        }

        // Colors
        {
            auto colors = style.find_node("Colors");

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
        }
    }

    return true;
}