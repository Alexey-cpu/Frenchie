#include <FrenchieCoreStringUtilities.hpp>

using namespace Frenchie::Core::String;

#include <algorithm>
#include <functional>

namespace Frenchie
{
    namespace Core
    {
        namespace String
        {
            namespace Internal
            {
                template<typename Type> 
                std::vector<std::basic_string<Type>> split_string(
                    const std::basic_string<Type>& _Input,
                    const std::basic_string<Type>& _Delimeter)
                {
                    if(_Input.empty())
                        return std::vector<std::basic_string<Type>>();

                    int start  = 0;
                    int end    = 0;
                    int size   = (int)_Delimeter.size();
                    std::vector<std::basic_string<Type>> output;

                    while(end >= 0)
                    {
                        end = (int)_Input.find(_Delimeter, start);
                        output.push_back(_Input.substr(start, end-start));
                        start = end + size;
                    }

                    return output;
                }

                template<typename Type> 
                bool string_contains_substring(const std::basic_string<Type>& _String, const std::basic_string<Type>& _Substring)
                {
                    auto iterator = std::search(
                        _String.begin(),
                        _String.end(), 
                        std::boyer_moore_searcher(_Substring.begin(), _Substring.end()));

                    return !_Substring.empty() && !_String.empty() && iterator != _String.end();
                }
            }
        }
    }
}

std::vector<std::string> Frenchie::Core::String::split_utf8_string(const std::string& _Input, const std::string& _Delimeter)
{
    return Internal::split_string<char>(_Input, _Delimeter);
}

std::vector<std::u16string> Frenchie::Core::String::split_utf16_string(const std::u16string& _Input, const std::u16string& _Delimeter)
{
    return Internal::split_string<char16_t>(_Input, _Delimeter);
}

std::vector<std::u32string> Frenchie::Core::String::split_utf32_string(const std::u32string& _Input, const std::u32string& _Delimeter)
{
    return Internal::split_string<char32_t>(_Input, _Delimeter);
}

bool Frenchie::Core::String::utf8_string_contains_substring(const std::string& _String, const std::string& _Substring)
{
    return Internal::string_contains_substring<char>(_String, _Substring);
}

bool Frenchie::Core::String::utf16_string_contains_substring(const std::u16string& _String, const std::u16string& _Substring)
{
    return Internal::string_contains_substring<char16_t>(_String, _Substring);
}

bool Frenchie::Core::String::utf32_string_contains_substring(const std::u32string& _String, const std::u32string& _Substring)
{
    return Internal::string_contains_substring<char32_t>(_String, _Substring);
}