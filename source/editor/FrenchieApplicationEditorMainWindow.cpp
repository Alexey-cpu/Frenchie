#include <FrenchieApplicationEditorMainWindow.hpp>
#include <FrenchieApplicationEditorConsoleLayer.hpp>
#include <FrenchieApplicationEditorMainMenuLayer.hpp>

#include <FrenchieCoreHelpers.hpp>

// SPDLOG
#include "spdlog/sinks/basic_file_sink.h"

// STL
#include <chrono>
#include <iostream>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

MainWindow::MainWindow()
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
        // create app logs directory
        if(!std::filesystem::exists(m_AppLogDirectory)) 
            std::filesystem::create_directory(m_AppLogDirectory);

        // create app state directory
        if(!std::filesystem::exists(m_AppStateDirectory)) 
            std::filesystem::create_directory(m_AppStateDirectory);
    }

    // clean-up logs if there are too many of them
    if(std::filesystem::exists(m_AppLogDirectory))
    {
        size_t logFilesNumber = 0;
        
        for(const auto& directory :
                std::filesystem::directory_iterator(m_AppLogDirectory.make_preferred(), std::filesystem::directory_options::skip_permission_denied))
        {
            logFilesNumber++;
        }

        if(logFilesNumber > 100) // TODO: this must be a setting !!!
            std::filesystem::remove_all(m_AppLogDirectory);
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
    application->push<Frenchie::Application::Editor::MainMenu>();
    application->push<Frenchie::Application::Editor::Console>();

    // log
    Frenchie::Core::Logger::instance()->info(fmt::format("App .exe directory: {}", Helpers::String::as_utf8(m_AppExeDirectory.wstring())));
    Frenchie::Core::Logger::instance()->info(fmt::format("App .ini directory: {}", Helpers::String::as_utf8(m_AppLogDirectory.wstring())));
    Frenchie::Core::Logger::instance()->info(fmt::format("App .xml directory: {}", Helpers::String::as_utf8(m_AppStateDirectory.wstring())));
    Frenchie::Core::Logger::instance()->info(fmt::format("App log file path: {}", logFileName));
}

MainWindow::~MainWindow()
{
    Frenchie::Application::Application::instance()->save_state(
        std::filesystem::path(m_AppStateDirectory.wstring().append(L"/State.xml"))
    );
}

int MainWindow::execute()
{
    return Frenchie::Application::Application::instance()->execute();
}

Reference<CommandsQueue> MainWindow::get_commands_queue()
{
    Reference<CommandsQueue> commandsQueue = 
        Frenchie::Application::Application::instance()->find<CommandsQueue>();

    if(commandsQueue == nullptr) 
        commandsQueue = Frenchie::Application::Application::instance()->push<CommandsQueue>();

    return commandsQueue;
}