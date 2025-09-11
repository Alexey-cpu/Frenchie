#include <FrenchieApplicationEditorLauncher.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

// Editor
#include <FrenchieEditorHelpers.hpp>

// configuration
#include <FrenchieEditorConfigurationTranslatorLayer.hpp>

// TODO: remove this when finished !!!
#include <FrenchieImGuiDemoLayer.hpp>

// Core
#include <FrenchieCoreHelpers.hpp>

// SPDLOG
#include "spdlog/sinks/basic_file_sink.h"

// STL
#include <chrono>
#include <iostream>
#include <fstream>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Editor;

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
                public Frenchie::Application::Command::Registry<ExitAction, void*>
            {
            public:

                ExitAction(void* _Sender = nullptr) : 
                    Frenchie::Application::Command::Registry<ExitAction, void*>(_Sender){}
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
        }
    }
}

std::filesystem::path Launcher::get_app_exe_directory()
{
    return Frenchie::Core::FileSystem::get_exe_absolute_directory();
}

std::filesystem::path Launcher::get_app_log_directory()
{
    return std::filesystem::path(
        Launcher::get_app_exe_directory().wstring().append(L"/appData/logs")).make_preferred();
}

std::filesystem::path Launcher::get_app_state_directory()
{
    return std::filesystem::path(
        Launcher::get_app_exe_directory().wstring().append(L"/appData/state")).make_preferred();
}

std::filesystem::path Launcher::get_app_translation_files_directory()
{
    return Launcher::get_app_exe_directory().wstring().append(L"/appData/translations");
}

int Launcher::execute()
{
    // create and configure application
    Frenchie::Application::application()->set_window_size(glm::vec2(2048, 1024));
    Frenchie::Application::application()->set_maximized(true);

    // create directories for logs and application state
    std::filesystem::path appExeDirectory              = Launcher::get_app_exe_directory();
    std::filesystem::path appLogDirectory              = Launcher::get_app_log_directory();
    std::filesystem::path appStateDirectory            = Launcher::get_app_state_directory();
    std::filesystem::path appTranslationFilesDirectory = Launcher::get_app_translation_files_directory();

    if(std::filesystem::exists(appExeDirectory))
    {
        // create app logs directory
        if(!std::filesystem::exists(appLogDirectory)) 
        {
            try
            {
                std::filesystem::create_directory(appLogDirectory);
            }
            catch(const std::exception& e)
            {
                Frenchie::Core::Logger::instance()->critical(e.what());
            }
        }

        // create app state directory
        if(!std::filesystem::exists(appStateDirectory))
        {
            try
            {
                std::filesystem::create_directory(appStateDirectory);
            }
            catch(const std::exception& e)
            {
                Frenchie::Core::Logger::instance()->critical(e.what());
            }
        }

        // create app translation files directory
        if(!std::filesystem::exists(appTranslationFilesDirectory))
        {
            try
            {
                std::filesystem::create_directory(appTranslationFilesDirectory);
            }
            catch(const std::exception& e)
            {
                Frenchie::Core::Logger::instance()->critical(e.what());
            }
        }
    }

    // clean-up logs if there are too many of them
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
    Frenchie::Core::Logger::instance()->register_sink<spdlog::sinks::stdout_color_sink_mt>();

    auto rawtime     = time(nullptr);
    auto localTime   = localtime(&rawtime);
    auto logFileName = Frenchie::Core::String::as_utf8(appLogDirectory.wstring())
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

    // load application state
    Frenchie::Application::application()->load_state(std::filesystem::path(appStateDirectory.wstring().append(L"/State.xml")).make_preferred());

    // append basic layers
    Frenchie::Application::application()->push_layer<Frenchie::Editor::MainMenu::Instance>();

    // configuration
    Frenchie::Application::application()->push_layer<Frenchie::Application::ImguiDemo>(); // FilesOpenDialog

    // log
    Frenchie::Core::Logger::instance()->info(fmt::format("App .exe directory: {}", Frenchie::Core::String::as_utf8(appExeDirectory.wstring())));
    Frenchie::Core::Logger::instance()->info(fmt::format("App .ini directory: {}", Frenchie::Core::String::as_utf8(appLogDirectory.wstring())));
    Frenchie::Core::Logger::instance()->info(fmt::format("App .xml directory: {}", Frenchie::Core::String::as_utf8(appStateDirectory.wstring())));
    Frenchie::Core::Logger::instance()->info(fmt::format("App .xml directory: {}", Frenchie::Core::String::as_utf8(appTranslationFilesDirectory.wstring())));
    Frenchie::Core::Logger::instance()->info(fmt::format("App log file path: {}", logFileName));

    // execute app and wait until it finishes it's job
    auto execution = Frenchie::Application::application()->execute();

    // save application state
    Frenchie::Application::application()->save_state(
        std::filesystem::path(Launcher::get_app_state_directory().wstring().append(L"/State.xml"))
    );

    return 1;
}