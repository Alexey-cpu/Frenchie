#include <FrenchieApplicationConfigurationLoaderFontsLayer.hpp>

// Core
#include <FrenchieCoreStringUnicode.hpp>
#include <FrenchieCoreFileSystem.hpp>
#include <FrenchieCoreThreadPool.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>
#include <FrenchieApplicationConfigurationLoader.hpp>

// STL
#include <chrono>
#include <iostream>
#include <fstream>
#include <functional>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Application::Configuration;

namespace Frenchie
{
    namespace Application
    {
        namespace Configuration
        {
            class PushFontsLoaderIntoAppQueue : 
                public Frenchie::Application::Command::Registry<PushFontsLoaderIntoAppQueue, const CommandPayloads&>
            {
            public:

                PushFontsLoaderIntoAppQueue(const CommandPayloads& _Sender) : 
                    Frenchie::Application::Command::Registry<PushFontsLoaderIntoAppQueue, const CommandPayloads&>(_Sender){}
                virtual ~PushFontsLoaderIntoAppQueue(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::Configuration::fonts();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Configuration), STRINGIFY(Fonts));
                }
            };

            const bool pushFontsLoaderIntoAppQueueStatus = PushFontsLoaderIntoAppQueue::registerFactory();
        }
    }
}

// Fonts
Fonts::Fonts() : Layer(STRINGIFY(Fonts)){}

Fonts::~Fonts(){}

std::filesystem::path Fonts::get_app_fonts_files_directory() const
{
    if(std::filesystem::exists(m_AppFontsFilesPath))
        return m_AppFontsFilesPath;

    auto configurationLoader = 
        Frenchie::Application::application()
            ->find_layer<Frenchie::Application::Configuration::ConfigurationLoader>();

    if(configurationLoader == nullptr) 
        return m_AppFontsFilesPath;
        
    m_AppFontsFilesPath = configurationLoader->get_app_data_directory().u32string().append(U"/fonts");

    if(!std::filesystem::exists(m_AppFontsFilesPath)) 
    {
        try
        {
            std::filesystem::create_directory(m_AppFontsFilesPath);
        }
        catch(...)
        {
            // TODO: put a log here...
        }
    }

    return m_AppFontsFilesPath;
}

bool Fonts::allows_multiple_instances() const 
{
    return false;
}

bool Fonts::serialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto main  = _Parent.append_node(STRINGIFY(Fonts));
    main.append_node("Size").set_value_as<float>(ImGui::GetStyle().FontSizeBase);
    main.append_node("Font", ImGui::GetFont()->GetDebugName());
    auto entries = main.append_node("Fonts");

    for(auto&& font : m_Paths)
    {
        entries.append_node(
            Frenchie::Core::String::convert_utf32_to_utf8(font.filename().stem().u32string()).c_str(), 
            Frenchie::Core::String::convert_utf32_to_utf8(font.u32string()).c_str());
    }

    return true;
}

bool Fonts::deserialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto main = _Parent.find_node("Fonts");

    // load fonts
    {
        auto fonts = main.find_node("Fonts");

        for(auto&& font : fonts)
        {
            std::filesystem::path path(font.get_value());

            if(std::filesystem::exists(path)) 
                m_Paths.insert(path);
        }

        // try to load from paths
        load_fonts(m_Paths, std::string(main.find_node("Font").get_value()));
    }

    // setup font size
    {
        auto node = main.find_node("Size");

        if(node.is_valid()) 
            ImGui::GetStyle().FontSizeBase = node.get_value_as<float>();
    }

    return true;
}

void Fonts::load_fonts(
    const std::set<std::filesystem::path>& _Fonts, 
    const std::string&                     _Font)
{
    auto loadFontsLambda = 
    [](const Frenchie::Application::Thread*    _Thread, 
        const std::set<std::filesystem::path>& _Fonts, 
        const std::string&                     _Font = std::string())->std::set<std::filesystem::path>
    {
        // load fonts
        if(_Fonts.empty())
            return std::set<std::filesystem::path>();

        // go on...
        std::set<std::filesystem::path> fonts;
        auto total   = _Fonts.size();
        auto current = 0;

        _Thread->push_message("Loading started...\n");

        for(auto&& path : _Fonts)
        {
            // wait on pause
            while(_Thread->paused() && !_Thread->stopped());

            // stop on pause
            if(_Thread->stopped()) 
                return fonts;

            _Thread->push_message(
                fmt::format("Trying to load font {}\n", 
                Frenchie::Core::String::convert_utf32_to_utf8(path.u32string())
            ));

            if(fonts.find(path) != fonts.end())
            {
                _Thread->set_progress((float)(++current) / (float)total);

                _Thread->push_message(
                    fmt::format("already loaded font {}\n", 
                    Frenchie::Core::String::convert_utf32_to_utf8(path.u32string())
                ));
                
                continue;
            }

            if(!std::filesystem::exists(path))
            {
                _Thread->set_progress((float)(++current) / (float)total);

                _Thread->push_message(
                    fmt::format("font at following path does not exist: {}\n", 
                    Frenchie::Core::String::convert_utf32_to_utf8(path.u32string())
                ));
                
                continue; 
            }

            // retrive ImGui IO
            auto& io = ImGui::GetIO();

            // load font
            try
            {
                io.Fonts->AddFontFromFileTTF(
                    Frenchie::Core::String::convert_utf32_to_utf8(path.u32string()).c_str(),
                    ImGui::GetStyle().FontSizeBase,
                    nullptr,
                    io.Fonts->GetGlyphRangesCyrillic());
            }
            catch(...)
            {
            }

            _Thread->set_progress((float)(++current) / (float)total);
            _Thread->push_message("font loading succeded...\n");

            // add to cache
            fonts.insert(path);
        }

        return fonts;
    };

    if(!m_ThreadsQueue.instance()->empty()) 
    {
        return;
    }

    // try to load from default paths
    auto loadFontsThread = m_ThreadsQueue.instance()->push(
        [loadFontsLambda, _Fonts, _Font, this](const Frenchie::Application::Thread* _Thread)
        {
            // try to load
            m_Paths = loadFontsLambda(_Thread, _Fonts, _Font);

            if(m_Paths.empty())
            {
                // try to load from default paths
                std::filesystem::path defaultFontsFilesPath = get_app_fonts_files_directory();

                if(std::filesystem::exists(defaultFontsFilesPath))
                {
                    std::set<std::filesystem::path> paths;

                    try
                    {
                        for(auto directory : std::filesystem::directory_iterator(defaultFontsFilesPath, 
                                            std::filesystem::directory_options::skip_permission_denied))
                        {
                            if(!directory.is_directory() &&
                                Frenchie::Core::FileSystem::get_file_extention(directory.path()) == ".ttf") 
                            {
                                paths.insert(directory.path());
                            }
                        }
                    }
                    catch(...)
                    {
                    }

                    m_Paths = loadFontsLambda(_Thread, paths);
                }
            }
        },
        "Load fonts...");

        // what we do on finish
        loadFontsThread->on_finished(
            [this, _Fonts, _Font](const Frenchie::Application::Thread*)
            {
                // build fonts and reload app
                ImGui::GetIO().Fonts->Build();
                Frenchie::Application::interface()->reload();

                for(ImFont* font : ImGui::GetIO().Fonts->Fonts)
                {
                    if(std::string(font->GetDebugName()) == _Font)
                    {
                        ImGui::GetIO().FontDefault = font;
                    }
                }
            }
        );
}

Frenchie::Core::Reference<Frenchie::Application::Configuration::Fonts> Frenchie::Application::Configuration::fonts()
{
    auto layer = Frenchie::Application::application()->find_layer<Fonts>();

    if(layer == nullptr) 
        layer = Frenchie::Application::application()->push_layer<Fonts>();

    return layer;
}