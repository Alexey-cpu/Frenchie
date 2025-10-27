#pragma once

#include <memory>
#include <string>
#include <bitset>
#include <codecvt>
#include <sstream>
#include <filesystem>
#include <functional>

namespace Frenchie
{
    namespace Core
    {
        namespace String
        {
            std::vector<std::string> split(const std::string& _Input, const std::string& _Delimeter = " ");
            std::string to_upper(std::string);
            std::string to_lower(std::string);

            std::string replace_symbol(std::string& _Input, const char& _From, const char& _To);
            std::string remove_symbol(std::string& _Input, const char& _What);
            std::string replace_substring(const std::string& _String, const std::string& _Substring, const std::string& _NewSubstring);
            bool contains_substring(const std::string& _String, const std::string& _Substring);
            bool contains_symbol(const std::string& _Input, char _Symbol = ' ');
        }

        namespace Time
        {
            inline std::chrono::high_resolution_clock::time_point tic()
            {
                return std::chrono::high_resolution_clock::now();
            }

            template< typename __type = std::chrono::nanoseconds >
            double elapsed(std::chrono::high_resolution_clock::time_point _Then, std::chrono::high_resolution_clock::time_point _Now)
            {
                return (double)std::chrono::duration_cast<__type>(_Now - _Then).count();
            }
        }  
    }
}