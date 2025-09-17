#include <FrenchieEditorConfigurationFontsLayer.hpp>

// Core
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreThreadPool.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

// Editor
#include <FrenchieEditorFileSystemExplorerLayer.hpp>
#include <FrenchieEditorConfigurationTranslatorLayer.hpp>

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
using namespace Frenchie::Editor::FileSystem;
using namespace Frenchie::Editor::Configuration;

namespace Frenchie
{
    namespace Editor
    {
        namespace Configuration
        {
            class PushFontsIntoAppQueue : 
                public Frenchie::Application::Command::Registry<PushFontsIntoAppQueue, void*>
            {
            public:

                PushFontsIntoAppQueue(void* _Sender) : 
                    Frenchie::Application::Command::Registry<PushFontsIntoAppQueue, void*>(_Sender){}
                virtual ~PushFontsIntoAppQueue(){}

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
        }
    }
}

LoadFontsProcess::LoadFontsProcess(const std::set<std::filesystem::path>& _Paths)
{
    for(auto&& path : _Paths)
    {
        if(std::filesystem::exists(path)) 
            m_Paths.insert(path);
    }
}

LoadFontsProcess::LoadFontsProcess(const std::filesystem::path& _Path)
{
    if(std::filesystem::exists(_Path)) 
        m_Paths.insert(_Path);
}

LoadFontsProcess::~LoadFontsProcess(){}

void LoadFontsProcess::execute()
{
    // cache user defined callback
    auto finished = m_OnFinished;

    on_finished(
        [this, finished]()
        {
            // build fonts and reload app
            ImGui::GetIO().Fonts->Build();
            Frenchie::Application::application()->reload();

            // launch user-defined callback
            if(finished != nullptr)
                finished();
        }
    );

    // load fonts
    if(m_Paths.empty())
    {
        m_Failed = true;
        return;
    }

    // go on...
    std::set<std::filesystem::path> fonts;
    auto total   = m_Paths.size();
    auto current = 0;

    for(auto&& path : m_Paths)
    {
        if(canceled()) 
            return;

        while (paused())
        {
            if(canceled()) 
                return;
        }

        if(!std::filesystem::exists(path) || 
            fonts.find(path) != fonts.end())
        {
            m_Progress = (float)(++current) / (float)total;
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
        catch(const std::exception& e)
        {
            Frenchie::Core::Logger::instance()->critical(e.what());
        }

        m_Progress = (float)(++current) / (float)total;

        // add to cache
        fonts.insert(path);
    }

    // finish
    m_Finished = true;
}

std::string LoadFontsProcess::iprocess_status_request_status()
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_Status;
}

float LoadFontsProcess::iprocess_progress_request_progress()
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_Progress;
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

        Frenchie::Application::ProcessQueue::instance()->push<LoadFontsProcess>(m_Paths)->on_failed(
            [this]()
            {
                Frenchie::Core::Logger::instance()->critical("NO FONTS FOUND !!!!");

                std::filesystem::path defaultFontsFilesPath = 
                    std::filesystem::path(Frenchie::Core::FileSystem::get_exe_absolute_directory().wstring().append(L"/appData/fonts/")).make_preferred();

                if(std::filesystem::exists(defaultFontsFilesPath))
                {
                    Frenchie::Core::Logger::instance()->critical("TRYING TO LOAD DEFAULT FONTS");

                    m_Paths.clear();

                    try
                    {
                        for(auto directory : std::filesystem::directory_iterator(defaultFontsFilesPath, 
                                            std::filesystem::directory_options::skip_permission_denied))
                        {
                            if(!directory.is_directory() &&
                                Frenchie::Core::FileSystem::get_file_extention(directory.path()) == ".ttf") 
                            {
                                m_Paths.insert(directory.path());
                            }
                        }

                        // once again
                        Frenchie::Application::ProcessQueue::instance()->push<LoadFontsProcess>(m_Paths);
                    }
                    catch(const std::exception& e)
                    {
                        Frenchie::Core::Logger::instance()->critical(e.what());
                    }
                }
            }
        );
    }

    return true;
}

void Fonts::load_fonts(
    const std::set<std::filesystem::path>& _Fonts, 
    const std::string&                     _Font)
{
    if(_Fonts.empty()) 
        return;

    Frenchie::Application::ProcessQueue::instance()
        ->push<Frenchie::Editor::Configuration::LoadFontsProcess>(_Fonts)
        ->on_finished([this, _Fonts](){m_Paths = _Fonts;});
}

Frenchie::Core::Reference<Fonts> Fonts::instance()
{
    auto layer = Frenchie::Application::application()->find_layer<Fonts>();

    if(layer == nullptr) 
        layer = Frenchie::Application::application()->push_layer<Fonts>();

    return layer;
}