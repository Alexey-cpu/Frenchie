#pragma once

#include <FrenchieApplication.hpp>

namespace Frenchie
{
    namespace Editor
    {
        class Launcher
        {
        public:

            // getters
            static std::filesystem::path get_app_exe_directory();
            static std::filesystem::path get_app_data_directory();
            static std::filesystem::path get_app_log_directory();
            static std::filesystem::path get_app_state_directory();
            static std::filesystem::path get_app_translation_files_directory();
            static std::filesystem::path get_app_fonts_files_directory();
            static std::filesystem::path get_app_themes_files_directory();

            static std::set<std::filesystem::path> get_app_data_directories();

            // API
            static int execute();
        };
    }
}