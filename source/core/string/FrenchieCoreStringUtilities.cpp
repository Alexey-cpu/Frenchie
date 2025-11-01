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
                std::vector<std::basic_string<Type>> split_basic_string(
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
                bool basic_string_contains_substring(const std::basic_string<Type>& _String, const std::basic_string<Type>& _Substring)
                {
                    auto iterator = std::search(
                        _String.begin(),
                        _String.end(), 
                        std::boyer_moore_searcher(_Substring.begin(), _Substring.end()));

                    return !_Substring.empty() && !_String.empty() && iterator != _String.end();
                }

                template<typename Type> 
                std::basic_string<Type> transform_basic_string_to_lower(std::basic_string<Type> _String)
                {
                    std::transform(_String.begin(), _String.end(), _String.begin(), ::tolower);
                    return _String;
                }

                template<typename Type> 
                std::basic_string<Type> transform_basic_string_to_upper(std::basic_string<Type> _String)
                {
                    std::transform(_String.begin(), _String.end(), _String.begin(), ::toupper);
                    return _String;
                }

                template<typename Type> 
                std::basic_string<Type> remove_symbols_from_basic_string(const std::basic_string<Type>& _String, const std::set<Type>& _Remove)
                {
                    std::basic_string<Type> output;
                    
                    for (auto&& symbol : _String)
                    {
                        if(_Remove.find(symbol) == _Remove.end())
                            output += symbol;
                    }

                    return output;
                }
            }
        }
    }
}

std::string    Frenchie::Core::String::utf8_to_lower(std::string _Input)
{
    return Internal::transform_basic_string_to_lower<char>(_Input);
}

std::u16string Frenchie::Core::String::utf16_to_lower(std::u16string _Input)
{
    return Internal::transform_basic_string_to_lower<char16_t>(_Input);
}

std::u32string Frenchie::Core::String::utf32_to_lower(std::u32string _Input)
{
    return Internal::transform_basic_string_to_lower<char32_t>(_Input);
}

std::string    Frenchie::Core::String::utf8_to_upper(std::string _Input)
{
    return Internal::transform_basic_string_to_upper<char>(_Input);
}

std::u16string Frenchie::Core::String::utf16_to_upper(std::u16string _Input)
{
    return Internal::transform_basic_string_to_upper<char16_t>(_Input);
}

std::u32string Frenchie::Core::String::utf32_to_upper(std::u32string _Input)
{
    return Internal::transform_basic_string_to_upper<char32_t>(_Input);
}

std::string    Frenchie::Core::String::utf8_remove_symbols(const std::string& _Input, const std::set<char>& _Remove)
{
    return Internal::remove_symbols_from_basic_string<char>(_Input, _Remove);
}

std::u16string Frenchie::Core::String::utf16_remove_symbols(const std::u16string& _Input, const std::set<char16_t>& _Remove)
{
    return Internal::remove_symbols_from_basic_string<char16_t>(_Input, _Remove);
}

std::u32string Frenchie::Core::String::utf32_remove_symbols(const std::u32string& _Input, const std::set<char32_t>& _Remove)
{
    return Internal::remove_symbols_from_basic_string<char32_t>(_Input, _Remove);
}

std::vector<std::string> Frenchie::Core::String::split_utf8_string(const std::string& _Input, const std::string& _Delimeter)
{
    return Internal::split_basic_string<char>(_Input, _Delimeter);
}

std::vector<std::u16string> Frenchie::Core::String::split_utf16_string(const std::u16string& _Input, const std::u16string& _Delimeter)
{
    return Internal::split_basic_string<char16_t>(_Input, _Delimeter);
}

std::vector<std::u32string> Frenchie::Core::String::split_utf32_string(const std::u32string& _Input, const std::u32string& _Delimeter)
{
    return Internal::split_basic_string<char32_t>(_Input, _Delimeter);
}

bool Frenchie::Core::String::utf8_string_contains_substring(const std::string& _String, const std::string& _Substring)
{
    return Internal::basic_string_contains_substring<char>(_String, _Substring);
}

bool Frenchie::Core::String::utf16_string_contains_substring(const std::u16string& _String, const std::u16string& _Substring)
{
    return Internal::basic_string_contains_substring<char16_t>(_String, _Substring);
}

bool Frenchie::Core::String::utf32_string_contains_substring(const std::u32string& _String, const std::u32string& _Substring)
{
    return Internal::basic_string_contains_substring<char32_t>(_String, _Substring);
}