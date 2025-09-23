#include <FrenchieEditorConfigurationLoaderFonts.hpp>

// Core
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreThreadPool.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

// Editor
#include <FrenchieApplicationEditorLauncher.hpp>

// SPDLOG
#include "spdlog/sinks/basic_file_sink.h"

// STL
#include <chrono>
#include <iostream>
#include <fstream>
#include <functional>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Editor;
using namespace Frenchie::Editor::Configuration;

namespace Frenchie
{
    namespace Editor
    {
        namespace Configuration
        {
            class PushFontsLoaderIntoAppQueue : 
                public Frenchie::Application::Command::Registry<PushFontsLoaderIntoAppQueue, void*>
            {
            public:

                PushFontsLoaderIntoAppQueue(void* _Sender) : 
                    Frenchie::Application::Command::Registry<PushFontsLoaderIntoAppQueue, void*>(_Sender){}
                virtual ~PushFontsLoaderIntoAppQueue(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Editor::Configuration::Fonts::instance();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::Configuration), STRINGIFY(Fonts));
                }
            };

            const bool pushFontsLoaderIntoAppQueueStatus = PushFontsLoaderIntoAppQueue::registerFactory();
        }
    }
}

// Fonts
Fonts::Fonts() : Layer(STRINGIFY(Fonts)){}

Fonts::~Fonts(){}

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
            Frenchie::Core::String::as_utf8(font.filename().stem().wstring()).c_str(), 
            Frenchie::Core::String::as_utf8(font.wstring()).c_str());
    }

    return true;
}

bool Fonts::deserialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto main = _Parent.find_node("Fonts");

    // size
    {
        auto node = main.find_node("Size");

        if(node.is_valid()) 
            ImGui::GetStyle().FontSizeBase = node.get_value_as<float>();
    }

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
        load_fonts(m_Paths);
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

        auto progress = _Thread->find_component<Frenchie::Application::ThreadProgressComponent>();
        auto status   = _Thread->find_component<Frenchie::Application::ThreadStatusComponent>();

        // go on...
        std::set<std::filesystem::path> fonts;
        auto total   = _Fonts.size();
        auto current = 0;

        if(status != nullptr) 
            status->push_message("Loading started...\n");

        for(auto&& path : _Fonts)
        {
            // wait on pause
            while(_Thread->paused() && !_Thread->stopped());

            // stop on pause
            if(_Thread->stopped()) 
                return fonts;

            if(status != nullptr) 
            {
                status->push_message(
                    fmt::format("Trying to load font {}\n", 
                    Frenchie::Core::String::as_utf8(path.wstring())
                ));
            }

            if(fonts.find(path) != fonts.end())
            {
                if(progress != nullptr)
                    progress->set_progress((float)(++current) / (float)total);

                status->push_message(
                    fmt::format("already loaded font {}\n", 
                    Frenchie::Core::String::as_utf8(path.wstring())
                ));
                
                continue;
            }

            if(!std::filesystem::exists(path))
            {
                if(progress != nullptr)
                    progress->set_progress((float)(++current) / (float)total);

                status->push_message(
                    fmt::format("font at following path does not exist: {}\n", 
                    Frenchie::Core::String::as_utf8(path.wstring())
                ));
                
                continue; 
            }

            // retrive ImGui IO
            auto& io = ImGui::GetIO();

            // load font
            try
            {
                io.Fonts->AddFontFromFileTTF(
                    Frenchie::Core::String::as_utf8(path.wstring()).c_str(),
                    ImGui::GetStyle().FontSizeBase,
                    nullptr,
                    io.Fonts->GetGlyphRangesCyrillic());
            }
            catch(...)
            {
            }

            if(progress != nullptr)
                progress->set_progress((float)(++current) / (float)total);

            if(status != nullptr) 
                status->push_message("font loading succeded...\n");

            // add to cache
            fonts.insert(path);
        }

        return fonts;
    };

    if(!m_ThreadsQueue.instance()->empty()) 
    {
        // TODO: add log here...
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
                std::filesystem::path defaultFontsFilesPath = 
                    Frenchie::Editor::Launcher::get_app_fonts_files_directory();

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
                    catch(const std::exception& e)
                    {
                        Frenchie::Core::Logger::instance()->critical(e.what());
                    }

                    m_Paths = loadFontsLambda(_Thread, paths);
                }
            }
        },
        "Load fonts...");

        // what we do on finish
        loadFontsThread->on_finished(
            [this, _Fonts](const Frenchie::Application::Thread*)
            {
                // build fonts and reload app
                ImGui::GetIO().Fonts->Build();
                Frenchie::Application::application()->reload();
            }
        );

        loadFontsThread->attach_component<Frenchie::Application::ThreadProgressComponent>();
        loadFontsThread->attach_component<Frenchie::Application::ThreadStatusComponent>();
}

Frenchie::Core::Reference<Fonts> Fonts::instance()
{
    auto layer = Frenchie::Application::application()->find_layer<Fonts>();

    if(layer == nullptr) 
        layer = Frenchie::Application::application()->push_layer<Fonts>();

    return layer;
}