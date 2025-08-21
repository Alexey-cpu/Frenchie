#include <FrenchieApplicationEditor.hpp>
#include <FrenchieCoreHelpers.hpp>

#include <FrenchieApplicationEditorConsoleLayer.hpp>
#include <FrenchieApplicationEditorMainMenuLayer.hpp>

// SPDLOG
#include "spdlog/sinks/basic_file_sink.h"

// STL
#include <chrono>
#include <iostream>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

Frenchie::Application::Editor::Editor::Editor()
{
    // create and configure application
    auto application = Frenchie::Application::Application::instance();
    application->set_window_size(glm::vec2(2048, 1024));
    application->set_maximized(true);

    // create directories for logs and application state
    m_AppExeDirectory   = Helpers::get_exe_absolute_directory();
    m_AppLogDirectory   = std::filesystem::path(m_AppExeDirectory.wstring().append(L"/appData/logs")).make_preferred();
    m_AppStateDirectory = std::filesystem::path(m_AppExeDirectory.wstring().append(L"/appData/state")).make_preferred();

    if(std::filesystem::exists(m_AppExeDirectory))
    {
        if(!std::filesystem::exists(m_AppLogDirectory)) 
            std::filesystem::create_directory(m_AppLogDirectory);

        if(!std::filesystem::exists(m_AppStateDirectory)) 
            std::filesystem::create_directory(m_AppStateDirectory);
    }

    // setup application logger
    Frenchie::Core::Logger::instance()->set_level(spdlog::level::level_enum::trace);
    Frenchie::Core::Logger::instance()->register_sink<spdlog::sinks::stdout_color_sink_mt>();

    auto rawtime     = time(nullptr);
    auto localTime   = localtime(&rawtime);
    auto logFileName = Helpers::String::as_utf8(m_AppLogDirectory.wstring())
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
    application->load_state(std::filesystem::path(m_AppStateDirectory.wstring().append(L"/State.xml")));

    // load application .ini file

    // append layers
    application->push<Frenchie::Application::Editor::Console>();
    application->push<Frenchie::Application::Editor::MainMenu>();

    // log
    Frenchie::Core::Logger::instance()->info(fmt::format("App .exe directory: {}", Helpers::String::as_utf8(m_AppExeDirectory.wstring())));
    Frenchie::Core::Logger::instance()->info(fmt::format("App .ini directory: {}", Helpers::String::as_utf8(m_AppLogDirectory.wstring())));
    Frenchie::Core::Logger::instance()->info(fmt::format("App .xml directory: {}", Helpers::String::as_utf8(m_AppStateDirectory.wstring())));
    Frenchie::Core::Logger::instance()->info(fmt::format("App log file path: {}", logFileName));
}

Frenchie::Application::Editor::Editor::~Editor()
{
    Frenchie::Application::Application::instance()->save_state(
        std::filesystem::path(m_AppStateDirectory.wstring().append(L"/State.xml"))
    );
}

int Frenchie::Application::Editor::Editor::execute()
{
    return Frenchie::Application::Application::instance()->execute();
}