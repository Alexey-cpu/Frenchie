#include <FrenchieApplicationEditorLauncher.hpp>

// Core
#include <FrenchieCoreStringUnicode.hpp>
#include <FrenchieCoreFileSystem.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

// Editor
#include <FrenchieEditorHelpers.hpp>
#include <FrenchieApplicationConfigurationLoader.hpp>

// TODO: remove this when finished !!!
#include <FrenchieImGuiDemoLayer.hpp>

// SPDLOG
#include "spdlog/sinks/basic_file_sink.h"

// STL
#include <chrono>
#include <iostream>
#include <fstream>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Editor;
using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Editor
    {
        namespace MainMenu
        {
            class Instance : public Frenchie::Application::Layer
            {
            public:
                Instance() : Layer(STRINGIFY(Instance)){}
                virtual ~Instance(){}

                // Frenchie::Application::Layer
                virtual void frame_update() override
                {
                    if(ImGui::BeginMainMenuBar())
                    {
                        Frenchie::Editor::Helpers::draw_menu(STRINGIFY(Frenchie::Editor::MainMenu));
                        ImGui::EndMainMenuBar();
                    }
                }
            };

            class ExitAction : 
                public Frenchie::Application::Command::Registry<ExitAction, const CommandPayloads&>
            {
            public:

                ExitAction(const CommandPayloads& _Sender = CommandPayloads()) : 
                    Frenchie::Application::Command::Registry<ExitAction, const CommandPayloads&>(_Sender){}
                virtual ~ExitAction(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->close();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Frenchie::Exit");
                }
            };

            const bool exitActionRegistry = ExitAction::registerFactory();
        }
    }
}

std::filesystem::path Launcher::get_app_exe_directory()
{
    return Frenchie::Core::FileSystem::get_exe_absolute_directory();
}

std::filesystem::path Launcher::get_app_data_directory()
{
    return std::filesystem::path(Launcher::get_app_exe_directory().wstring().append(L"/appData")).make_preferred(); 
}

std::filesystem::path Launcher::get_app_log_directory()
{
    return std::filesystem::path(Launcher::get_app_data_directory().wstring().append(L"/logs")).make_preferred();
}

// std::filesystem::path Launcher::get_app_state_directory()
// {
//     return std::filesystem::path(Launcher::get_app_data_directory().wstring().append(L"/state")).make_preferred();
// }

std::set<std::filesystem::path> Launcher::get_app_data_directories()
{
    return
    {
        get_app_log_directory(),
        //get_app_state_directory()
    };
}

int Launcher::execute()
{
    // create and configure application
    Frenchie::Application::platform()->set_maximized(true);

    // create app state directories
    auto appDataDirectories = get_app_data_directories();

    for(auto&& appDataDirectory : appDataDirectories )
    {
        if(!std::filesystem::exists(appDataDirectory)) 
        {
            try
            {
                std::filesystem::create_directory(appDataDirectory);
            }
            catch(const std::exception& e)
            {
                Frenchie::Core::Logger::instance()->critical(e.what());
            }
        }
    }

    // clean-up logs if there are too many of them
    std::filesystem::path appLogDirectory = Launcher::get_app_log_directory();

    if(std::filesystem::exists(appLogDirectory))
    {
        std::vector<std::filesystem::path> paths;
        
        for(const auto& directory :
                std::filesystem::directory_iterator(appLogDirectory, std::filesystem::directory_options::skip_permission_denied))
        {
            paths.push_back(directory.path());
        }

        if(paths.size() > 100)
        {
            for(auto&& path : paths) 
            {
                try
                {
                    std::filesystem::remove_all(path);
                }
                catch(const std::exception& e)
                {
                    Frenchie::Core::Logger::instance()->critical(e.what());
                }
            }
        }
    }

    // setup application logger
    Frenchie::Core::Logger::instance()->set_level(spdlog::level::level_enum::trace);

    try
    {
        Frenchie::Core::Logger::instance()->register_sink<spdlog::sinks::stdout_color_sink_mt>();
    }
    catch(const std::exception& e)
    {
        Frenchie::Core::Logger::instance()->info(e.what());
    }

    try
    {
        auto rawtime     = time(nullptr);
        auto localTime   = localtime(&rawtime);
        auto logFileName = Frenchie::Core::String::convert_utf32_to_utf8(appLogDirectory.u32string())
            .append("/")
            .append(fmt::format("protocol_{}_{}_{}_{}_{}_{}_{}.txt", 
                localTime->tm_mday,
                localTime->tm_mon  + 1,
                localTime->tm_year + 1900,
                localTime->tm_hour,
                localTime->tm_min,
                localTime->tm_sec,
                rawtime
            )
        );

        Frenchie::Core::Logger::instance()->register_sink<spdlog::sinks::basic_file_sink_mt>(logFileName, true);
    }
    catch(const std::exception& e)
    {
        Frenchie::Core::Logger::instance()->info(e.what());
    }

    Frenchie::Core::Logger::instance()->info("Factory registry...");
    for (auto&& creator : Frenchie::Core::Factory::registry())
    {
        Frenchie::Core::Logger::instance()->info(creator.first);
    }

    // setup main menu
    Frenchie::Application::application()->push_layer<Frenchie::Editor::MainMenu::Instance>();

    // load configuration
    Frenchie::Application::application()->push_layer<Frenchie::Application::Configuration::ConfigurationLoader>(
        std::filesystem::path(get_app_data_directory().make_preferred()));
    Frenchie::Application::application()->push_layer<Frenchie::Application::ImguiDemo>(); // FilesOpenDialog

    // log
    Frenchie::Core::Logger::instance()->info(fmt::format("App .exe file directory: {}", Frenchie::Core::String::convert_utf32_to_utf8(get_app_exe_directory().u32string())));
    Frenchie::Core::Logger::instance()->info(fmt::format("App .xml state directory: {}", Frenchie::Core::String::convert_utf32_to_utf8(get_app_data_directory().u32string())));

    return Frenchie::Application::application()->execute();
}