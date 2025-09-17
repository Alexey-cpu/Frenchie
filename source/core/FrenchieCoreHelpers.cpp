#include <FrenchieCoreHelpers.hpp>

using namespace Frenchie::Core;

#include <functional>
#include <algorithm>

// Windows fucking API
#ifdef _MSC_VER
#include <windows.h>
#include <libloaderapi.h>
#else
#include <unistd.h>
#endif

#if defined(__APPLE__) || defined(MACH)
#include <mach-o/dyld.h>
#endif

// PUGIXML
#include <pugixml.hpp>

// FMT
#include <fmt/format.h>

// STL
#include <array>

// Frenchie::Core::FileSystem
std::filesystem::path Frenchie::Core::FileSystem::get_exe_absolute_path()
{
    #ifdef IS_WINDOWS
        wchar_t path[FILENAME_MAX] = { 0 };
        GetModuleFileNameW(nullptr, path, FILENAME_MAX);
        return std::filesystem::path(path);
    #endif

    #ifdef IS_LINUX
        char path[FILENAME_MAX];
        ssize_t count = readlink("/proc/self/exe", path, FILENAME_MAX);
        return std::filesystem::path(std::string(path, (count > 0) ? count: 0));
    #endif

    #ifdef IS_MACOS
        char path[FILENAME_MAX];
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) == 0)
            return std::filesystem::path(std::string(path));
        return std::filesystem::path();
    #endif
}

std::filesystem::path Frenchie::Core::FileSystem::get_exe_absolute_directory()
{
    return get_exe_absolute_path().parent_path();
}

std::string Frenchie::Core::FileSystem::get_file_extention(const std::filesystem::path& _Path)
{
    if(std::filesystem::is_directory(_Path)) 
        return std::string();

    auto        filePath  = std::filesystem::path(_Path).make_preferred().stem();
    std::string extention = std::filesystem::path(_Path).make_preferred().extension().string();

    while (!filePath.extension().empty())
    {
        extention = filePath.extension().string().append(extention);
        filePath  = std::filesystem::path(filePath.stem().wstring());
    }

    return extention;
}

FILE* Frenchie::Core::FileSystem::open_file(const std::string& _Path, std::string _Mode)
{
    auto to_wstring = [](const std::string _Value)->std::wstring
    {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(_Value);
    };

    FILE* file = std::fopen(_Path.c_str(), _Mode.c_str());

    #if defined(_WIN32) || defined(WIN32) // try to do something on Windows
        if(file == nullptr)
            file = _wfopen(&to_wstring(_Path)[0], &to_wstring(_Mode)[0] );
    #endif

    return file;
}

void Frenchie::Core::FileSystem::create_directory(
    const std::filesystem::path& _Path, 
    const std::function<void()>& _OnSuccess, 
    const std::function<void(const std::exception&)>& _OnFail)
{
    if(!std::filesystem::exists(_Path)) 
    {
        try
        {
            std::filesystem::create_directory(_Path);

            if(_OnSuccess != nullptr) 
                _OnSuccess();
        }
        catch(const std::exception& e)
        {
            if(_OnFail != nullptr) 
                _OnFail(e);
        }
    }
}

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
        return _Input == "true" || std::stoi( _Input ) ? true : false;
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

// TODO: inplement custom UTF8, UTF16 e.t.c encoders
std::string Frenchie::Core::String::as_utf8(const std::wstring& _Input)
{
    return pugi::as_utf8(_Input);
}

std::wstring Frenchie::Core::String::as_wide(const std::string& _Input)
{
    return pugi::as_wide(_Input);
}