#pragma once

#include <filesystem>
#include <functional>

namespace Frenchie
{
    namespace Core
    {
        namespace FileSystem
        {
            std::filesystem::path get_exe_absolute_path();

            std::filesystem::path get_exe_absolute_directory();

            std::string get_file_extention(const std::filesystem::path& _Path);

            FILE* open_file(const std::u32string& _Path, std::u32string _Mode);

            void create_directory(
                const std::filesystem::path& _Path, 
                const std::function<void()>& _OnSuccess, 
                const std::function<void(const std::exception&)>& _OnFail);
        }
    }
}