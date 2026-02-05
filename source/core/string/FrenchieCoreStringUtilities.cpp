#include <FrenchieCoreStringUtilities.hpp>

using namespace Frenchie::Core::String;

// STL
#include <chrono>
#include <algorithm>
#include <functional>

#include <utf8.h>

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
                    #if __cplusplus == 201703L
                    auto iterator = std::search(
                        _String.begin(),
                        _String.end(), 
                        std::boyer_moore_searcher(_Substring.begin(), _Substring.end()));

                    return !_Substring.empty() && !_String.empty() && iterator != _String.end();
                    #else
                    return _String.find(_Substring) != std::string::npos;
                    #endif
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

template<> float Frenchie::Core::String::from_string<float>(const std::string& _Input)
{
    try
    {
        return std::stof(_Input);
    }
    catch(...)
    {
        return 0.0f;
    }
}

template<> double Frenchie::Core::String::from_string<double>(const std::string& _Input)
{
    try
    {
        return std::stod(_Input);
    }
    catch(...)
    {
        return 0.0;
    }
}

template<> long double Frenchie::Core::String::from_string<long double>(const std::string& _Input)
{
    try
    {
        return std::stold(_Input);
    }
    catch(...)
    {
        return 0.0;
    }
}

template<> short Frenchie::Core::String::from_string<short>(const std::string& _Input)
{
    try
    {
        return std::stoi(_Input);
    }
    catch(...)
    {
        return 0;
    }
}

template<> int Frenchie::Core::String::from_string<int>(const std::string& _Input)
{
    try
    {
        return std::stoi(_Input);
    }
    catch(...)
    {
        return 0;
    }
}

template<> long Frenchie::Core::String::from_string<long>(const std::string& _Input)
{
    try
    {
        return std::stol(_Input);
    }
    catch(...)
    {
        return 0;
    }
}

template<> long long Frenchie::Core::String::from_string<long long>(const std::string& _Input)
{
    try
    {
        return std::stoll(_Input);
    }
    catch(...)
    {
        return 0;
    }
}

template<> unsigned short Frenchie::Core::String::from_string<unsigned short>(const std::string& _Input)
{
    try
    {
        return (unsigned short)std::stoul(_Input);
    }
    catch(...)
    {
        return 0;
    }
}

template<> unsigned int Frenchie::Core::String::from_string<unsigned int>(const std::string& _Input)
{
    try
    {
        return std::stoul(_Input);
    }
    catch(...)
    {
        return 0;
    }
}

template<> unsigned long Frenchie::Core::String::from_string<unsigned long>(const std::string& _Input)
{
    try
    {
        return std::stoul(_Input);
    }
    catch(...)
    {
        return 0;
    }
}

template<> unsigned long long Frenchie::Core::String::from_string<unsigned long long>(const std::string& _Input)
{
    try
    {
        return std::stoull(_Input);
    }
    catch(...)
    {
        return 0;
    }
}

template<> bool Frenchie::Core::String::from_string<bool>(const std::string& _Input)
{
    try
    {
        return !_Input.empty() && ((utf8_to_lower(_Input) == "true") || std::stoi(_Input) > 0) ? true : false;
    }
    catch(...)
    {
        return false;
    }
}

template<> std::string Frenchie::Core::String::from_string< std::string >(const std::string& _Input)
{
    return _Input;
}

template<> char Frenchie::Core::String::from_string<char>(const std::string& _Input)
{
    return _Input.empty() ? ' ' : _Input[0];
}

template<> std::string Frenchie::Core::String::to_string<bool>(const bool& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<float>(const float& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<double>(const double& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<long double>(const long double& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<short>(const short& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<int>(const int& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<long>(const long& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<long long>(const long long& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<unsigned short>(const unsigned short& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<unsigned int>(const unsigned int& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<unsigned long>(const unsigned long& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<unsigned long long>(const unsigned long long& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::String::to_string<nullptr_t>(const nullptr_t& _Input)
{
    return "0";
}

template<> std::string Frenchie::Core::String::to_string<std::string>(const std::string& _Input)
{
    return _Input;
}

template<> std::string Frenchie::Core::String::to_string<std::chrono::system_clock::time_point>(const std::chrono::system_clock::time_point& _Input)
{
    std::time_t now = std::chrono::system_clock::to_time_t(_Input);
    std::string time = std::string(std::asctime(std::localtime(&now)));
    time.pop_back();
    return time;
}

std::string Frenchie::Core::String::convert_utf16_to_utf8(const std::u16string& _Input)
{
    std::string result;
    utf8::unchecked::utf16to8(_Input.begin(), _Input.end(), std::back_inserter(result));
    return result;
}

std::string Frenchie::Core::String::convert_utf32_to_utf8(const std::u32string& _Input)
{
    std::string result;
    utf8::unchecked::utf32to8(_Input.begin(), _Input.end(), std::back_inserter(result));
    return result;
}

std::u16string Frenchie::Core::String::convert_utf8_to_utf16(const std::string& _Input)
{
    std::u16string result;
    utf8::unchecked::utf8to16(_Input.begin(), _Input.end(), std::back_inserter(result));
    return result;
}

std::u32string Frenchie::Core::String::convert_utf8_to_utf32(const std::string& _Input)
{
    std::u32string result;
    utf8::unchecked::utf8to32(_Input.begin(), _Input.end(), std::back_inserter(result));
    return result;
}