#include <FrenchieApplicationEditorLauncher.hpp>

// Frenchie::Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationEditorConsoleLayer.hpp>
#include <FrenchieApplicationEditorMainMenuLayer.hpp>
#include <FrenchieApplicationEditorPreferencesLayer.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

// TODO: remove this when finished !!!
#include <FrenchieImGuiDemoLayer.hpp>

// Frenchie::Core
#include <FrenchieCoreHelpers.hpp>

// SPDLOG
#include "spdlog/sinks/basic_file_sink.h"

// STL
#include <chrono>
#include <iostream>
#include <fstream>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;


std::filesystem::path Launcher::get_app_exe_directory()
{
    return Helpers::get_exe_absolute_directory();
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

std::filesystem::path Launcher::get_app_console_directory()
{
    return std::filesystem::path(
        Launcher::get_app_exe_directory().wstring().append(L"/appData/console")).make_preferred();
}

std::filesystem::path Launcher::get_app_console_log_file_path()
{
    return Launcher::get_app_console_directory().wstring().append(L"/console.txt");
}

std::string Launcher::get_system_path_variable(const std::string& _Name)
{
    // remove 'console.txt' file
    try
    {
        std::filesystem::remove_all(Launcher::get_app_console_log_file_path());
    }
    catch(const std::exception& e)
    {
        Frenchie::Core::Logger::instance()->critical(e.what());
    }

    // write system PATH variable contents into a new 'console.txt' file
#ifdef _WIN32
    Frenchie::Core::Helpers::launch_command(
        "echo", 
        fmt::format("%{}%", _Name).c_str(), 
        Frenchie::Core::Helpers::String::as_utf8(Launcher::get_app_console_log_file_path()).c_str()
    );
#else
    Frenchie::Core::Helpers::launch_command(
        "echo", 
        fmt::format("${}", _Name).c_str(), 
        Frenchie::Core::Helpers::String::as_utf8(Launcher::get_app_console_log_file_path()).c_str()
    );
#endif

    // read 'console.txt'
    std::ifstream ifsream(Launcher::get_app_console_log_file_path());

    return std::string(
            (std::istreambuf_iterator<char>(ifsream)), 
            (std::istreambuf_iterator<char>()));
}

int Launcher::execute()
{
    // create and configure application
    auto application = Frenchie::Application::Application::instance();
    application->set_window_size(glm::vec2(2048, 1024));
    application->set_maximized(true);

    // create directories for logs and application state
    std::filesystem::path appExeDirectory     = Launcher::get_app_exe_directory();
    std::filesystem::path appLogDirectory     = Launcher::get_app_log_directory();
    std::filesystem::path appStateDirectory   = Launcher::get_app_state_directory();
    std::filesystem::path appConsoleDirectory = Launcher::get_app_console_directory();

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

        // create app console directory
        if(!std::filesystem::exists(appConsoleDirectory))
        {
            try
            {
                std::filesystem::create_directory(appConsoleDirectory);
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
    auto logFileName = Helpers::String::as_utf8(appLogDirectory.wstring())
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
    application->load_state(std::filesystem::path(appStateDirectory.wstring().append(L"/State.xml")).make_preferred());

    // append basic layers
    application->push<Frenchie::Application::Editor::Console>();
    application->push<Frenchie::Application::Editor::MainMenu>();
    application->push<Frenchie::Application::Editor::Preferences>();
    application->push<Frenchie::Application::Editor::FileSystemExplorer>("Exp-1");
    application->push<Frenchie::Application::Editor::FileSystemExplorer>("Exp-2");

    application->push<Frenchie::Application::ImguiDemo>();

    // LOAD FONTS
    // TODO: this command MUST BE pushed from application settings
    // application->find_or_push<CommandsQueue>()->push<CallbackCommand>(
    //     []()
    //     {
    //         int m_DefaultFontSize = 16;
    //         std::filesystem::path m_Path = "C:/SDK/Qt_Projects/OpenGL/shared/fonts";

    //         // retrive ImGui IO
    //         auto& io = ImGui::GetIO();
    //         io.Fonts->Clear();

    //         // recursivelly scan path for .ttf fonts
    //         for(const auto& directory :
    //             std::filesystem::recursive_directory_iterator(m_Path, std::filesystem::directory_options::skip_permission_denied))
    //         {
    //             if(directory.is_directory() ||
    //                 directory.path().extension() != ".ttf")
    //                 continue;

    //             io.Fonts->AddFontFromFileTTF(
    //                 pugi::as_utf8(directory.path().wstring()).c_str(),
    //                 m_DefaultFontSize * (4.0 / 3.0),
    //                 nullptr,
    //                 io.Fonts->GetGlyphRangesCyrillic());
    //         }

    //         // build fonts
    //         io.Fonts->Build();

    //         //reload app
    //         Frenchie::Application::Application::instance()->reload();
    //     }
    // );

    // log
    Frenchie::Core::Logger::instance()->info(fmt::format("App .exe directory: {}", Helpers::String::as_utf8(appExeDirectory.wstring())));
    Frenchie::Core::Logger::instance()->info(fmt::format("App .ini directory: {}", Helpers::String::as_utf8(appLogDirectory.wstring())));
    Frenchie::Core::Logger::instance()->info(fmt::format("App .xml directory: {}", Helpers::String::as_utf8(appStateDirectory.wstring())));
    Frenchie::Core::Logger::instance()->info(fmt::format("App .xml directory: {}", Helpers::String::as_utf8(appConsoleDirectory.wstring())));
    Frenchie::Core::Logger::instance()->info(fmt::format("App log file path: {}", logFileName));

    // execute app and wait until it finishes it's job
    auto execution = Frenchie::Application::Application::instance()->execute();

    // save application state
    Frenchie::Application::Application::instance()->save_state(
        std::filesystem::path(Launcher::get_app_state_directory().wstring().append(L"/State.xml"))
    );

    return 1;
}