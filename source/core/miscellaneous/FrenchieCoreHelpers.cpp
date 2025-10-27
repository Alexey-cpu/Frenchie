#include <FrenchieCoreHelpers.hpp>

using namespace Frenchie::Core;

#include <functional>
#include <algorithm>

// FMT
#include <fmt/format.h>

// STL
#include <array>

// Frenchie::Core::String
std::vector<std::string> Frenchie::Core::String::split(const std::string& _Input, const std::string& _Delimeter)
{
    if( _Input.empty() )
        return std::vector<std::string>();

    int start  = 0;
    int end    = 0;
    int size   = (int)_Delimeter.size();
    std::vector<std::string> output;

    while(end >= 0)
    {
        end = (int)_Input.find(_Delimeter, start);
        output.push_back( _Input.substr(start, end-start) );
        start = end + size;
    }

    return output;
}

std::string Frenchie::Core::String::to_upper(std::string _String)
{
    std::transform(_String.begin(), _String.end(), _String.begin(), ::toupper);
    return _String;
}

std::string Frenchie::Core::String::to_lower(std::string _String)
{
    std::transform(_String.begin(), _String.end(), _String.begin(), ::tolower);
    return _String;
}

std::string Frenchie::Core::String::replace_symbol(std::string& _Input, const char& _From, const char& _To)
{
    std::string result;

    for(int i = 0; i < (int)_Input.size(); i++)
        result.push_back(_Input[i] == _From ? _To: _Input[i]);
    
    return result;
}

std::string Frenchie::Core::String::remove_symbol(std::string& _Input, const char& _What)
{
    std::string result;

    for(int i = 0; i < (int)_Input.size(); i++)
    {
        if(_Input[i] != _What)
            result.push_back(_Input[i]);
    }
    
    return result;
}

std::string Frenchie::Core::String::replace_substring(const std::string& _String, const std::string& _Substring, const std::string& _NewSubstring)
{
    // reserve buffer for substring
    std::string buffer;
    buffer.reserve(_Substring.size());

    // generate output string
    std::string output;
    output.reserve(_String.size());

    // main code
    for(size_t i = 0; i < _String.size(); )
    {
        for(size_t j = 0, k = i; j < _Substring.size(); j++, k++)
            buffer.push_back(_String[k]);

        if(buffer == _Substring)
        {
            output.append(_NewSubstring);
            i += _Substring.size();
        }
        else
        {
            output.push_back(_String[i]);
            i++;
        }

        buffer.clear();
    }

    return output;
}

bool Frenchie::Core::String::contains_substring(const std::string& _String, const std::string& _Substring)
{
    auto iterator = std::search(
        _String.begin(),
        _String.end(), 
        std::boyer_moore_searcher(_Substring.begin(), _Substring.end()));

    return !_Substring.empty() && !_String.empty() && iterator != _String.end();
}

bool Frenchie::Core::String::contains_symbol(const std::string& _Input, char _Symbol)
{
    for(size_t i = 0; i < _Input.size(); i++)
    {
        if( _Input[i] == _Symbol )
            return true;
    }

    return false;
}