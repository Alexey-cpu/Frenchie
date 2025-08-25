#include <FrenchieApplicationEditorMainWindow.hpp>
#include <FrenchieApplicationEditorConsoleLayer.hpp>
#include <FrenchieApplicationEditorMainMenuLayer.hpp>

#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

#include <FrenchieCoreHelpers.hpp>

#include <FrenchieApplication.hpp>

// SPDLOG
#include "spdlog/sinks/basic_file_sink.h"

// STL
#include <chrono>
#include <iostream>

// IMGUI
#include <imgui.h>

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
    application->push<Frenchie::Application::Editor::FileSystem::Explorer>();

    // TODO: this command MUST BE pushed from application settings
    application->find_or_push<CommandsQueue>()->push<CallbackCommand>(
        [this]()
        {
            int m_DefaultFontSize = 16;
            std::filesystem::path m_Path = "C:/SDK/Qt_Projects/OpenGL/shared/fonts";

            // retrive ImGui IO
            auto& io = ImGui::GetIO();
            io.Fonts->Clear();

            // recursivelly scan path for .ttf fonts
            for(const auto& directory :
                std::filesystem::recursive_directory_iterator(m_Path, std::filesystem::directory_options::skip_permission_denied))
            {
                if(directory.is_directory() ||
                    directory.path().extension() != ".ttf")
                    continue;

                io.Fonts->AddFontFromFileTTF(
                    pugi::as_utf8(directory.path().wstring()).c_str(),
                    m_DefaultFontSize * (4.0 / 3.0),
                    nullptr,
                    io.Fonts->GetGlyphRangesCyrillic());
            }

            // build fonts
            io.Fonts->Build();

            //reload app
            Frenchie::Application::Application::instance()->reload();
        }
    );

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