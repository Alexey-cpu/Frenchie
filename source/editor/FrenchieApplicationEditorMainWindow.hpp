#pragma once

#include <FrenchieApplication.hpp>

#include <FrenchieApplicationCommandsQueueLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class MainWindow
            {
            public:
                MainWindow();
                ~MainWindow();
                
                // API
                int execute();

                // static API
                static Frenchie::Core::Reference<Frenchie::Application::CommandsQueue> get_commands_queue();


            protected:
                std::filesystem::path m_AppExeDirectory;
                std::filesystem::path m_AppLogDirectory;
                std::filesystem::path m_AppStateDirectory;
            };
        }
    }
}