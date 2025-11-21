#pragma once

#include <string>
#include <vector>
#include <set>

#define STRINGIFY(_VALUE) #_VALUE

namespace Frenchie
{
    namespace Core
    {
        namespace String
        {
            std::string    utf8_to_lower(std::string _Input);
            std::u16string utf16_to_lower(std::u16string _Input);
            std::u32string utf32_to_lower(std::u32string _Input);

            std::string    utf8_to_upper(std::string _Input);
            std::u16string utf16_to_upper(std::u16string _Input);
            std::u32string utf32_to_upper(std::u32string _Input);
         
            std::string    utf8_remove_symbols(const std::string& _Input, const std::set<char>& _Remove);
            std::u16string utf16_remove_symbols(const std::u16string& _Input, const std::set<char16_t>& _Remove);
            std::u32string utf32_remove_symbols(const std::u32string& _Input, const std::set<char32_t>& _Remove);

            std::vector<std::string>    split_utf8_string (const std::string&    _Input, const std::string&    _Delimeter);
            std::vector<std::u16string> split_utf16_string(const std::u16string& _Input, const std::u16string& _Delimeter);
            std::vector<std::u32string> split_utf32_string(const std::u32string& _Input, const std::u32string& _Delimeter);

            bool utf8_string_contains_substring(const std::string&    _String, const std::string&    _Substring);
            bool utf16_string_contains_substring(const std::u16string& _String, const std::u16string& _Substring);
            bool utf32_string_contains_substring(const std::u32string& _String, const std::u32string& _Substring);

            template<typename Type> 
            Type from_string(const std::string& _Input);

            // to string conversion
            template <typename Type>
            std::string to_string(const Type& _Input);
        }
    }
}