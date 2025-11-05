#include <FrenchieEditorPreferencesExplorerStyle.hpp>

// Core
#include <FrenchieCoreFileSystem.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

// Editor
#include <FrenchieApplicationEditorDialog.hpp>
#include <FrenchieApplicationConfigurationLoaderFontsLayer.hpp>
#include <FrenchieApplicationConfigurationLoaderThemesLayer.hpp>
#include <FrenchieApplicationConfigurationLoaderTranslatorLayer.hpp>
#include <FrenchieEditorFileSystemExplorerDialog.hpp>
#include <FrenchieEditorFileSystemExplorerPathScannerDialog.hpp>

using namespace Frenchie::Application;
using namespace Frenchie::Application::Configuration;

using namespace Frenchie::Editor;
using namespace Frenchie::Editor::Preferences;

// Add to main menu
namespace Frenchie
{
    namespace Editor
    {
        namespace MainMenu
        {
            class OpenStyleSettingsAction : 
                public Frenchie::Application::Command::Registry<OpenStyleSettingsAction, const CommandPayloads&>
            {
            public:

                OpenStyleSettingsAction(const CommandPayloads& _Sender = CommandPayloads()) : 
                    Frenchie::Application::Command::Registry<OpenStyleSettingsAction, const CommandPayloads&>(_Sender){}
                
                virtual ~OpenStyleSettingsAction(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->push_layer<Frenchie::Editor::Preferences::Style>()->show();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Windows::Preferences::Style settings");
                }
            };

            const bool openStyleSettingsAction = OpenStyleSettingsAction::registerFactory();
        }
    }
}

namespace Frenchie
{
    namespace Editor
    {
        namespace Preferences
        {
            class SelectBuiltInTheme : public Dialog
            {
            public:
                SelectBuiltInTheme() : 
                    Dialog(translator()->translate("Select built-in theme...")){}

                virtual ~SelectBuiltInTheme(){}

                virtual bool awake() override
                {
                    m_ReferenceStyle = ImGui::GetStyle();
                    return true;
                }

                // Frenchie::Application::Editor::Dialog
                virtual void draw_content() override
                {
                    if (ImGui::Combo("##", &m_StyleIndex, "Dark\0Light\0Classic\0"))
                    {
                        switch (m_StyleIndex)
                        {
                            case 0:
                            {
                                ImGui::StyleColorsDark(); 
                                break;
                            }

                            case 1: 
                            {
                                ImGui::StyleColorsLight(); 
                                break;
                            }

                            case 2: 
                            {
                                ImGui::StyleColorsClassic(); 
                                break;
                            }
                        }
                    }
                }

                virtual void draw_buttons() override
                {
                    if(ImGui::Button(translator()->translate("Ok").c_str()))
                    {
                        m_Accepted = true;
                        close();
                    }
                    
                    ImGui::SameLine();
                    
                    if(ImGui::Button(translator()->translate("Cancel").c_str()))
                    {
                        ImGui::GetStyle() = m_ReferenceStyle;
                        close();
                    }
                }

                virtual void finish() override
                {
                    if(m_Accepted) 
                        m_OnAccepted();
                }

                void on_accepted(const std::function<void()>& _Callback)
                {
                    m_OnAccepted = _Callback;
                }
                
            protected:

                int        m_StyleIndex = -1;
                ImGuiStyle m_ReferenceStyle;
                std::function<void()> m_OnAccepted;

                bool m_Accepted{false};
            };
        }
    }
}

// Style
Style::Style() : 
    Frenchie::Application::Layer(translator()->translate("Style settings")){}

Style::~Style(){}

bool Style::awake()
{
    m_ReferenceStyle = ImGui::GetStyle();
    return true;
}

void Style::frame_update() 
{
    // draw
    ImGui::Begin(fmt::format("{}###Style settings", translator()->translate(get_name())).c_str(), &m_Opened);
    {
        if (ImGui::TreeNode(translator()->translate("Application layout settings").c_str()))
        {
            draw_geometry_settings();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode(translator()->translate("Application color scheme settings").c_str()))
        {
            draw_color_settings();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode(translator()->translate("Application font settings").c_str()))
        {
            draw_fonts_settings();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode(translator()->translate("Application renderer settings").c_str()))
        {
            draw_rendering_settings();
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

bool Style::allows_multiple_instances() const
{
    return false;
}

void Style::draw_geometry_settings()
{
    auto& style = ImGui::GetStyle();

    ImGui::SeparatorText(translator()->translate("Main layout settings").c_str());
    {
        ImGui::SliderFloat2(translator()->translate("WindowPadding").c_str(), (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2(translator()->translate("FramePadding").c_str(), (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2(translator()->translate("ItemSpacing").c_str(), (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2(translator()->translate("ItemInnerSpacing").c_str(), (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2(translator()->translate("TouchExtraPadding").c_str(), (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
        ImGui::SliderFloat(translator()->translate("IndentSpacing").c_str(), &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
        ImGui::SliderFloat(translator()->translate("ScrollbarSize").c_str(), &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
        ImGui::SliderFloat(translator()->translate("GrabMinSize").c_str(), &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
    }

    ImGui::SeparatorText(translator()->translate("Borders layout settings").c_str());
    {
        {
            bool checked = style.WindowBorderSize > 0.0;
            if(ImGui::Checkbox(translator()->translate("WindowBorder").c_str(), &checked))
                style.WindowBorderSize = (float)(checked);
        }

        {
            bool checked = style.ChildBorderSize > 0.0;
            if(ImGui::Checkbox(translator()->translate("ChildBorder").c_str(), &checked)) 
                style.ChildBorderSize = (float)(checked);
        }

        {
            bool checked = style.PopupBorderSize > 0.0;
            if(ImGui::Checkbox(translator()->translate("PopupBorder").c_str(), &checked)) 
                style.PopupBorderSize = (float)(checked);
        }

        {
            bool checked = style.FrameBorderSize > 0.0;
            if(ImGui::Checkbox(translator()->translate("FrameBorder").c_str(), &checked))
                style.FrameBorderSize = (float)(checked);
        }

        {
            bool checked = style.TabBorderSize > 0.0f;
            if(ImGui::Checkbox(translator()->translate("TabBorder").c_str(), &checked))
                style.TabBorderSize = (float)(checked);
        }

        {
            bool checked = style.TabBarBorderSize > 0.0f;
            if(ImGui::Checkbox(translator()->translate("TabBarBorder").c_str(), &checked))
                style.TabBarBorderSize = (float)(checked);
        }
    }

    ImGui::SeparatorText(translator()->translate(translator()->translate("Layout rounding settings").c_str()).c_str());
    {
        ImGui::SliderFloat(translator()->translate("WindowRounding").c_str(), &style.WindowRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat(translator()->translate("ChildRounding").c_str(), &style.ChildRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat(translator()->translate("FrameRounding").c_str(), &style.FrameRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat(translator()->translate("PopupRounding").c_str(), &style.PopupRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat(translator()->translate("ScrollbarRounding").c_str(), &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat(translator()->translate("GrabRounding").c_str(), &style.GrabRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat(translator()->translate("TabRounding").c_str(), &style.TabRounding, 0.0f, 12.0f, "%.0f");
    }

    ImGui::SeparatorText(translator()->translate("Tabs layout settings").c_str());
    {
        ImGui::SliderFloat(translator()->translate("TabBarOverlineSize").c_str(), &style.TabBarOverlineSize, 0.0f, 3.0f, "%.0f");
        ImGui::DragFloat(translator()->translate("TabMinWidthBase").c_str(), &style.TabMinWidthBase, 0.5f, 1.0f, 500.0f, "%.0f");
        ImGui::DragFloat(translator()->translate("TabMinWidthShrink").c_str(), &style.TabMinWidthShrink, 0.5f, 1.0f, 500.0f, "%0.f");

        {
            bool checked = style.TabCloseButtonMinWidthSelected < 0.0f;
            if(ImGui::Checkbox(translator()->translate("ShowCloseButtonOnSelectedTab").c_str(), &checked))
                style.TabCloseButtonMinWidthSelected = (float)(checked) * (-1.f);
        }

        {
            bool checked = style.TabCloseButtonMinWidthUnselected < 0.0f;
            if(ImGui::Checkbox(translator()->translate("ShowCloseButtonOnUnselectedTab").c_str(), &checked))
                style.TabCloseButtonMinWidthUnselected = (float)(checked) * (-1.f); 
        }
    }

    ImGui::SeparatorText(translator()->translate("Tables layout settings").c_str());
    {
        ImGui::SliderFloat2(translator()->translate("CellPadding").c_str(), (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2(translator()->translate("TableAngledHeadersTextAlign").c_str(), (float*)&style.TableAngledHeadersTextAlign, 0.0f, 1.0f, "%.2f");
        ImGui::SliderAngle(translator()->translate("TableAngledHeadersAngle").c_str(), &style.TableAngledHeadersAngle, -50.0f, +50.0f);
    }

    ImGui::SeparatorText(translator()->translate("Trees layout settings").c_str());
    {
        if (ImGui::BeginCombo(translator()->translate("TreeLinesFlags").c_str(), GetTreeLinesFlagsName(style.TreeLinesFlags)))
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
        ImGui::SliderFloat(translator()->translate("TreeLinesSize").c_str(), &style.TreeLinesSize, 0.0f, 2.0f, "%.0f");
        ImGui::SliderFloat(translator()->translate("TreeLinesRounding").c_str(), &style.TreeLinesRounding, 0.0f, 12.0f, "%.0f");
    }

    ImGui::SeparatorText(translator()->translate("Dock windows geometry settings").c_str());
    {
        ImGui::SliderFloat(translator()->translate("DockingSeparatorSize").c_str(), &style.DockingSeparatorSize, 0.0f, 12.0f, "%.0f");
    }
}

void Style::draw_color_settings()
{
    auto& style = ImGui::GetStyle();

    auto themes       = Frenchie::Application::Configuration::themes()->get_supported_themes();
    auto currentTheme = Frenchie::Application::Configuration::themes()->get_current_theme();

    if(ImGui::BeginCombo(
        "##", 
        (currentTheme != nullptr ? currentTheme->get_name().c_str() : translator()->translate("There are not preconfigured themes...").c_str())))
    {
        for(auto&& theme : themes)
        {
            bool current = theme->is_current();
            if(ImGui::Checkbox(theme->get_name().c_str(), &current)) 
                theme->setup();

            ImGui::SetItemTooltip(Frenchie::Core::String::convert_utf32_to_utf8(theme->get_path().u32string()).c_str());
        }

        ImGui::EndCombo();
    }

    // button callbacks
    auto onBrowseAction = [this]()
    {
        auto dialog = Frenchie::Application::application()->push_layer<FileSystem::PathScannerDialog>(
            [](const std::filesystem::path& _Path)->bool
            {
                return !std::filesystem::is_directory(_Path) && 
                    Frenchie::Core::FileSystem::get_file_extention(_Path) == ".theme";
            },
            translator()->translate("Select path where to search for themes ...")
        );

        dialog->on_accepted(
            [this](const std::map<std::filesystem::path, bool>& _Paths)
            {
                std::set<std::filesystem::path> paths;

                for(auto&& path : _Paths) 
                {
                    if(path.second) 
                        paths.insert(path.first);
                }

                Frenchie::Application::Configuration::themes()->set_supported_themes(paths);
            }
        );
    };

    auto onSaveAction = [this]()
    {
        auto currentTheme = Frenchie::Application::Configuration::themes()->get_current_theme();
    
        if(currentTheme != nullptr) 
            currentTheme->save();
    };

    auto onSaveAsAction = [this]()
    {
        Frenchie::Application::application()->push_layer<Frenchie::Editor::FileSystem::ExplorerDialog>(
            translator()->translate("Select folder where to save theme ...")
        )->on_accepted(
            [this]()
            {
                Frenchie::Core::Reference<Frenchie::Editor::FileSystem::ExplorerDialog> dialog = 
                    Frenchie::Application::application()->find_layer<Frenchie::Editor::FileSystem::ExplorerDialog>();

                if(dialog == nullptr) 
                    return;

                auto currentPath = dialog->get_current_path();
                auto currentFile = dialog->get_current_file();

                if(!std::filesystem::exists(currentPath) || 
                    currentFile.u32string().empty()) 
                    return;

                Frenchie::Application::Configuration::themes()->create_theme(
                    std::filesystem::path(
                        currentPath.u32string().append(U"/").append(currentFile.filename().u32string())
                    ).make_preferred()
                )->save();
            }
        );
    };

    auto onSelectBuiltInThemeAction = 
    [this, onSaveAsAction]()
    {
        Frenchie::Application::application()->push_layer<SelectBuiltInTheme>()->on_accepted(
            [this, onSaveAsAction]()
            {
                // suggest user to save a selected built in style
                onSaveAsAction();
            }
        );
    };

    ImGui::SameLine();
    if(ImGui::Button(translator()->translate("Browse").c_str())) 
        onBrowseAction();

    ImGui::SameLine();
    if(ImGui::Button("Save"))
        onSaveAction();
    
    ImGui::SameLine();
    if(ImGui::Button(translator()->translate("Save as").c_str()))
        onSaveAsAction();

    ImGui::SameLine();

    if (ImGui::Button(translator()->translate("Select built-in theme").c_str()))
        onSelectBuiltInThemeAction();

    m_ColorFilter.Draw(translator()->translate(translator()->translate("Filter colors").c_str()).c_str());

    ImGui::SetNextWindowSizeConstraints(
        ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), 
        ImVec2(FLT_MAX, FLT_MAX));
    
    ImGui::BeginChild(
        "##colors", 
        ImVec2(0, 0), 
        ImGuiChildFlags_Borders | 
        ImGuiChildFlags_NavFlattened, 
        ImGuiWindowFlags_AlwaysVerticalScrollbar | 
        ImGuiWindowFlags_AlwaysHorizontalScrollbar);
    
    ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

    for (int i = 0; i < ImGuiCol_COUNT; i++)
    {
        std::string name = get_style_color_name(i);
        
        if (!m_ColorFilter.PassFilter(name.c_str()))
            continue;
        ImGui::PushID(i);
        
        ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar);
        
        // apply / revert functionality...
        if (memcmp(&style.Colors[i], &m_ReferenceStyle.Colors[i], sizeof(ImVec4)) != 0)
        {
            ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); 
            
            if(ImGui::Button(translator()->translate("Save").c_str())) 
                m_ReferenceStyle.Colors[i] = style.Colors[i];
            
            ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); 
            
            if (ImGui::Button(translator()->translate("Revert").c_str())) 
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
        for(ImFont* font : io.Fonts->Fonts)
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

    if(ImGui::Button(translator()->translate("Browse").c_str()))
    {
        auto dialog = Frenchie::Application::application()->push_layer<FileSystem::PathScannerDialog>(
            [](const std::filesystem::path& _Path)->bool
            {
                return !std::filesystem::is_directory(_Path) && 
                    Frenchie::Core::FileSystem::get_file_extention(_Path) == ".ttf";
            },
            translator()->translate("Select path where to search for fonts ...")
        );

        dialog->on_accepted(
            [this](const std::map<std::filesystem::path, bool>& _Paths)
            {
                std::set<std::filesystem::path> paths;

                for(auto&& path : _Paths) 
                {
                    if(path.second) 
                        paths.insert(path.first);
                }

                Frenchie::Application::Configuration::fonts()->load_fonts(paths);
            }
        );
    }

    // font size
    if (ImGui::DragFloat(translator()->translate("Size").c_str(), &ImGui::GetStyle().FontSizeBase, 0.20f, 5.0f, 100.0f, "%.0f"))
        ImGui::GetStyle()._NextFrameFontSizeBase = ImGui::GetStyle().FontSizeBase;
}

void Style::draw_rendering_settings()
{
    auto& style = ImGui::GetStyle();

    ImGui::Checkbox(translator()->translate("Antialiased lines").c_str(), &style.AntiAliasedLines);
    ImGui::Checkbox(translator()->translate("Antialiased lines use texture").c_str(), &style.AntiAliasedLinesUseTex);
    ImGui::Checkbox(translator()->translate("Antialiased lines fill").c_str(), &style.AntiAliasedFill);
    ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
    ImGui::DragFloat(translator()->translate("Curve tessellation tolerance").c_str(), &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
    if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;

    // When editing the "Circle Segment Max Error" value, draw a preview of its effect on auto-tessellated circles.
    ImGui::DragFloat(translator()->translate("Circle tessellation maximum error").c_str(), &style.CircleTessellationMaxError , 0.005f, 0.10f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    const bool show_samples = ImGui::IsItemActive();
    if (show_samples)
        ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());

    if (show_samples && ImGui::BeginTooltip())
    {
        ImGui::TextUnformatted(translator()->translate("R = radius, N = approx number of segments").c_str());
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

    ImGui::DragFloat(translator()->translate("Global alpha").c_str(), &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
    ImGui::DragFloat(translator()->translate("Disabled alpha").c_str(), &style.DisabledAlpha, 0.005f, 0.0f, 1.0f, "%.2f"); 
    ImGui::PopItemWidth();
}

std::string Style::get_style_color_name(ImGuiCol idx)
{
    // Create switch-case from enum with regexp: ImGuiCol_{.*}, --> case ImGuiCol_\1: return "\1";
    switch (idx)
    {
    case ImGuiCol_Text: return translator()->translate("Text");
    case ImGuiCol_TextDisabled: return translator()->translate("TextDisabled");
    case ImGuiCol_WindowBg: return translator()->translate("WindowBg");
    case ImGuiCol_ChildBg: return translator()->translate("ChildBg");
    case ImGuiCol_PopupBg: return translator()->translate("PopupBg");
    case ImGuiCol_Border: return translator()->translate("Border");
    case ImGuiCol_BorderShadow: return translator()->translate("BorderShadow");
    case ImGuiCol_FrameBg: return translator()->translate("FrameBg");
    case ImGuiCol_FrameBgHovered: return translator()->translate("FrameBgHovered");
    case ImGuiCol_FrameBgActive: return translator()->translate("FrameBgActive");
    case ImGuiCol_TitleBg: return translator()->translate("TitleBg");
    case ImGuiCol_TitleBgActive: return translator()->translate("TitleBgActive");
    case ImGuiCol_TitleBgCollapsed: return translator()->translate("TitleBgCollapsed");
    case ImGuiCol_MenuBarBg: return translator()->translate("MenuBarBg");
    case ImGuiCol_ScrollbarBg: return translator()->translate("ScrollbarBg");
    case ImGuiCol_ScrollbarGrab: return translator()->translate("ScrollbarGrab");
    case ImGuiCol_ScrollbarGrabHovered: return translator()->translate("ScrollbarGrabHovered");
    case ImGuiCol_ScrollbarGrabActive: return translator()->translate("ScrollbarGrabActive");
    case ImGuiCol_CheckMark: return translator()->translate("CheckMark");
    case ImGuiCol_SliderGrab: return translator()->translate("SliderGrab");
    case ImGuiCol_SliderGrabActive: return translator()->translate("SliderGrabActive");
    case ImGuiCol_Button: return translator()->translate("Button");
    case ImGuiCol_ButtonHovered: return translator()->translate("ButtonHovered");
    case ImGuiCol_ButtonActive: return translator()->translate("ButtonActive");
    case ImGuiCol_Header: return translator()->translate("Header");
    case ImGuiCol_HeaderHovered: return translator()->translate("HeaderHovered");
    case ImGuiCol_HeaderActive: return translator()->translate("HeaderActive");
    case ImGuiCol_Separator: return translator()->translate("Separator");
    case ImGuiCol_SeparatorHovered: return translator()->translate("SeparatorHovered");
    case ImGuiCol_SeparatorActive: return translator()->translate("SeparatorActive");
    case ImGuiCol_ResizeGrip: return translator()->translate("ResizeGrip");
    case ImGuiCol_ResizeGripHovered: return translator()->translate("ResizeGripHovered");
    case ImGuiCol_ResizeGripActive: return translator()->translate("ResizeGripActive");
    case ImGuiCol_InputTextCursor: return translator()->translate("InputTextCursor");
    case ImGuiCol_TabHovered: return translator()->translate("TabHovered");
    case ImGuiCol_Tab: return translator()->translate("Tab");
    case ImGuiCol_TabSelected: return translator()->translate("TabSelected");
    case ImGuiCol_TabSelectedOverline: return translator()->translate("TabSelectedOverline");
    case ImGuiCol_TabDimmed: return translator()->translate("TabDimmed");
    case ImGuiCol_TabDimmedSelected: return translator()->translate("TabDimmedSelected");
    case ImGuiCol_TabDimmedSelectedOverline: return translator()->translate("TabDimmedSelectedOverline");
    case ImGuiCol_DockingPreview: return translator()->translate("DockingPreview");
    case ImGuiCol_DockingEmptyBg: return translator()->translate("DockingEmptyBg");
    case ImGuiCol_PlotLines: return translator()->translate("PlotLines");
    case ImGuiCol_PlotLinesHovered: return translator()->translate("PlotLinesHovered");
    case ImGuiCol_PlotHistogram: return translator()->translate("PlotHistogram");
    case ImGuiCol_PlotHistogramHovered: return translator()->translate("PlotHistogramHovered");
    case ImGuiCol_TableHeaderBg: return translator()->translate("TableHeaderBg");
    case ImGuiCol_TableBorderStrong: return translator()->translate("TableBorderStrong");
    case ImGuiCol_TableBorderLight: return translator()->translate("TableBorderLight");
    case ImGuiCol_TableRowBg: return translator()->translate("TableRowBg");
    case ImGuiCol_TableRowBgAlt: return translator()->translate("TableRowBgAlt");
    case ImGuiCol_TextLink: return translator()->translate("TextLink");
    case ImGuiCol_TextSelectedBg: return translator()->translate("TextSelectedBg");
    case ImGuiCol_TreeLines: return translator()->translate("TreeLines");
    case ImGuiCol_DragDropTarget: return translator()->translate("DragDropTarget");
    case ImGuiCol_NavCursor: return translator()->translate("NavCursor");
    case ImGuiCol_NavWindowingHighlight: return translator()->translate("NavWindowingHighlight");
    case ImGuiCol_NavWindowingDimBg: return translator()->translate("NavWindowingDimBg");
    case ImGuiCol_ModalWindowDimBg: return translator()->translate("ModalWindowDimBg");
    }

    return translator()->translate("Unknown");
}

const char* Style::GetTreeLinesFlagsName(ImGuiTreeNodeFlags flags)
{
    if (flags == ImGuiTreeNodeFlags_DrawLinesNone) return "DrawLinesNone";
    if (flags == ImGuiTreeNodeFlags_DrawLinesFull) return "DrawLinesFull";
    if (flags == ImGuiTreeNodeFlags_DrawLinesToNodes) return "DrawLinesToNodes";
    return "";
}