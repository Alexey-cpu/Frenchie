#pragma once

#include <string>
#include <vector>

namespace Frenchie
{
    namespace Core
    {
        namespace String
        {
            std::vector<std::string>    split_utf8_string (const std::string&    _Input, const std::string&    _Delimeter);
            std::vector<std::u16string> split_utf16_string(const std::u16string& _Input, const std::u16string& _Delimeter);
            std::vector<std::u32string> split_utf32_string(const std::u32string& _Input, const std::u32string& _Delimeter);

            bool utf8_string_contains_substring(const std::string&    _String, const std::string&    _Substring);
            bool utf16_string_contains_substring(const std::u16string& _String, const std::u16string& _Substring);
            bool utf32_string_contains_substring(const std::u32string& _String, const std::u32string& _Substring);
        }
    }
}