#pragma once

#include <FrenchieApplication.hpp>

// This static class is needed to initialize application layers, logger
// and folders where to spawn application state, logs e.t.c

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class Launcher
            {
            public:

                // getters
                static std::filesystem::path get_app_exe_directory();
                static std::filesystem::path get_app_log_directory();
                static std::filesystem::path get_app_state_directory();
                static std::filesystem::path get_app_console_directory();
                static std::filesystem::path get_app_console_log_file_path();

                // API
                static int execute();
            };
        }
    }
}