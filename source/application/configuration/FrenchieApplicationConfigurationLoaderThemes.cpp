#include <FrenchieApplicationConfigurationLoaderThemes.hpp>

// Core
#include <FrenchieCoreFileSystem.hpp>
#include <FrenchieCoreStringUnicode.hpp>
#include <FrenchieCoreSerializationNode.hpp>
#include <FrenchieCoreSerializationFormatXML.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>
#include <FrenchieApplicationConfigurationLoader.hpp>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Core;

using namespace Frenchie::Application;
using namespace Frenchie::Application::Configuration;

namespace Frenchie
{
    namespace Editor
    {
        namespace Application
        {
            class PushThemesIntoAppQueue : 
                public Frenchie::Application::Command::Registry<PushThemesIntoAppQueue, const CommandPayloads&>
            {
            public:

                PushThemesIntoAppQueue(const CommandPayloads& _Sender) : 
                    Frenchie::Application::Command::Registry<PushThemesIntoAppQueue, const CommandPayloads&>(_Sender){}

                virtual ~PushThemesIntoAppQueue(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::Configuration::Themes::instance();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Configuration), STRINGIFY(Themes));
                }
            };

            const bool pushThemesIntoAppQueueStatus = PushThemesIntoAppQueue::registerFactory();
        }
    }
}

Theme::Theme(const std::filesystem::path& _Path, const Themes* _Owner) : 
    m_Path(_Path), 
    m_Owner(_Owner){}

Theme::~Theme(){}

std::string Theme::get_name() const
{
    return Frenchie::Core::String::convert_utf32_to_utf8(m_Path.filename().stem().u32string());
}

std::filesystem::path Theme::get_path() const
{
    return m_Path;
}

bool Theme::is_current() const
{
    return m_Current;
}

void Theme::setup()
{
    if(m_Owner == nullptr) 
        return;

    // deselect all
    for(auto&& theme : m_Owner->m_Themes)
        theme.second->m_Current = false;

    // select self
    m_Current = true;

    // load theme from file
    Frenchie::Core::Serialization::Document document;

    if(!document.read<Frenchie::Core::Serialization::XMLReader>(m_Path)) 
        return;

    // parse geometry
    auto geometry = document.find_node("Geometry");

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
    auto colors = document.find_node("Colors");

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

void Theme::save()
{
    // create a document
    Frenchie::Core::Serialization::Document document;

    // Main
    auto geometry = document.append_node("Geometry");
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
    auto colors = document.append_node("Colors");
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

    document.write<Frenchie::Core::Serialization::XMLBeautifulWriter>(m_Path);
}

// Themes
Themes::Themes() : Layer(STRINGIFY(Themes)){}
Themes::~Themes(){}

std::filesystem::path Themes::get_app_theme_files_path() const
{
    if(std::filesystem::exists(m_AppThemeFilesPath))
        return m_AppThemeFilesPath;

    auto configurationLoader = 
        Frenchie::Application::application()
            ->find_layer<Frenchie::Application::Configuration::ConfigurationLoader>();

    if(configurationLoader == nullptr) 
        return m_AppThemeFilesPath;
        
    m_AppThemeFilesPath = configurationLoader->get_app_data_path().wstring().append(L"/fonts");

    if(!std::filesystem::exists(m_AppThemeFilesPath)) 
    {
        try
        {
            std::filesystem::create_directory(m_AppThemeFilesPath);
        }
        catch(...)
        {
            // TODO: put a log here...
        }
    }

    return m_AppThemeFilesPath;
}

Frenchie::Core::Reference<Theme> Themes::get_current_theme() const
{
    for(auto&& theme : m_Themes) 
    {
        if(theme.second->is_current()) 
            return Frenchie::Core::Reference<Theme>(theme.second);
    }

    return Frenchie::Core::Reference<Theme>();
}

std::vector<Frenchie::Core::Reference<Theme>> Themes::get_supported_themes() const
{
    std::vector<Frenchie::Core::Reference<Theme>> themes;

    for(auto&& theme : m_Themes) 
        themes.push_back(theme.second);

    return themes;
}

void Themes::set_supported_themes(const std::set<std::filesystem::path>& _Paths)
{
    if(_Paths.empty()) 
        return;

    // clear
    m_Themes.clear();

    // add new themes
    for(auto&& path : _Paths)
    {
        if(std::filesystem::exists(path) && 
            Frenchie::Core::FileSystem::get_file_extention(path) == ".theme")
            m_Themes.insert({path, std::make_unique<Theme>(path, this)});
    }

    // setup first theme
    if(!m_Themes.empty()) 
        m_Themes.begin()->second->setup();
}

bool Themes::allows_multiple_instances() const
{
    return false;
}

bool Themes::serialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto main = _Parent.append_node(STRINGIFY(Themes));

    if(m_Themes.empty()) 
        return true;

    auto themes       = main.append_node("Themes");
    auto currentTheme = main.append_node("CurrentTheme");

    for(auto&& theme : m_Themes)
    {
        themes.append_node(
            theme.second->get_name().c_str(), 
            theme.second->get_path().string().c_str());

        if(theme.second->is_current())
            currentTheme.set_value(theme.second->get_name().c_str());
    }

    return true;
}

bool Themes::deserialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto main = _Parent.find_node(STRINGIFY(Themes));

    if(!main.is_valid())
        return false;

    // retrieve supported languages
    auto themes = main.find_node("Themes");

    if(themes.is_valid())
    {
        for(auto&& theme : themes)
        {
            if(!theme.is_valid() || 
                std::string(theme.get_name()).empty() || 
                !std::filesystem::exists(std::string(theme.get_value()))) 
                continue;

            auto path = std::filesystem::path(theme.get_value()).make_preferred();
            m_Themes.insert(
                {
                    path, 
                    std::make_unique<Theme>(path, this)
                }
            );
        }
    }
    else // try to to load theme files from default path
    {
        std::filesystem::path defaultTranslationFilesPath = get_app_theme_files_path();

        if(std::filesystem::exists(defaultTranslationFilesPath))
        {
            try
            {
                for(auto directory : std::filesystem::directory_iterator(defaultTranslationFilesPath, 
                                     std::filesystem::directory_options::skip_permission_denied))
                {
                    if(!directory.is_directory() &&
                        Frenchie::Core::FileSystem::get_file_extention(directory.path()) == ".theme") 
                    {
                        m_Themes.insert(
                            {
                                directory.path(),
                                std::make_unique<Theme>(directory.path(), this)
                            });
                    }
                }
            }
            catch(...){}
        }
    }

    // select current language
    auto currentLanguage = main.find_node("CurrentTheme");

    if(currentLanguage.is_valid() &&
        !std::string(currentLanguage.get_name()).empty() && 
        !std::string(currentLanguage.get_value()).empty()) 
    {
        for (auto&& supportedLanguage : m_Themes)
        {
            if(supportedLanguage.second->get_name() == 
                std::string(currentLanguage.get_value()))
            {
                supportedLanguage.second->setup();
                break;
            }
        }
    }
    else // try to setup default language
    {
        if(!m_Themes.empty())
            m_Themes.begin()->second->setup();
    }

    return true;
}

Frenchie::Core::Reference<Theme> Themes::create_theme(const std::filesystem::path& _Path)
{
    auto path  = std::filesystem::path(_Path.parent_path().wstring().append(L"/").append(_Path.filename().stem().wstring()).append(L".theme")).make_preferred();
    auto theme = std::make_shared<Theme>(path, this);
    m_Themes.insert({path, theme});

    return Frenchie::Core::Reference<Theme>(theme);
}

Frenchie::Core::Reference<Themes> Themes::instance()
{
    auto layer = Frenchie::Application::application()->find_layer<Themes>();

    if(layer == nullptr) 
        layer = Frenchie::Application::application()->push_layer<Themes>();

    return layer;
}