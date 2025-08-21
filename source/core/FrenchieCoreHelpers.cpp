#include <FrenchieCoreHelpers.hpp>

using namespace Frenchie::Core::Helpers;

#ifdef _MSC_VER
#include <windows.h>
#include <libloaderapi.h>
#endif

#include <pugixml.hpp>

std::filesystem::path Frenchie::Core::Helpers::get_exe_absolute_path()
{
    #if defined(_MSC_VER)
        wchar_t path[FILENAME_MAX] = { 0 };
        GetModuleFileNameW(nullptr, path, FILENAME_MAX);
        return std::filesystem::path(path);
    #else
        char path[FILENAME_MAX];
        ssize_t count = readlink("/proc/self/exe", path, FILENAME_MAX);
        return std::filesystem::path(std::string(path, (count > 0) ? count: 0));
    #endif
}

std::filesystem::path Frenchie::Core::Helpers::get_exe_absolute_directory()
{
    #if defined(_MSC_VER)
        wchar_t path[FILENAME_MAX] = { 0 };
        GetModuleFileNameW(nullptr, path, FILENAME_MAX);
        return std::filesystem::path(path).parent_path().string();
    #else
        char path[FILENAME_MAX];
        ssize_t count = readlink("/proc/self/exe", path, FILENAME_MAX);
        return std::filesystem::path(std::string(path, (count > 0) ? count: 0)).parent_path().string();
    #endif
}

std::string Frenchie::Core::Helpers::get_file_extention(const std::filesystem::path& _Path)
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

FILE* Frenchie::Core::Helpers::open_file(std::string _Path, std::string _Mode)
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

// Frenchie::Core::Helpers::String
std::vector<std::string> Frenchie::Core::Helpers::String::split(const std::string& _Input, const std::string& _Delimeter)
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

std::string Frenchie::Core::Helpers::String::to_upper(std::string _String)
{
    std::transform(_String.begin(), _String.end(), _String.begin(), ::toupper);
    return _String;
}

std::string Frenchie::Core::Helpers::String::to_lower(std::string _String)
{
    std::transform(_String.begin(), _String.end(), _String.begin(), ::tolower);
    return _String;
}

template<> float Frenchie::Core::Helpers::String::from_string<float>(const std::string& _Input)
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

template<> double Frenchie::Core::Helpers::String::from_string<double>(const std::string& _Input)
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

template<> long double Frenchie::Core::Helpers::String::from_string<long double>(const std::string& _Input)
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

template<> short Frenchie::Core::Helpers::String::from_string<short>(const std::string& _Input)
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

template<> int Frenchie::Core::Helpers::String::from_string<int>(const std::string& _Input)
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

template<> long Frenchie::Core::Helpers::String::from_string<long>(const std::string& _Input)
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

template<> long long Frenchie::Core::Helpers::String::from_string<long long>(const std::string& _Input)
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

template<> unsigned short Frenchie::Core::Helpers::String::from_string<unsigned short>(const std::string& _Input)
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

template<> unsigned int Frenchie::Core::Helpers::String::from_string<unsigned int>(const std::string& _Input)
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

template<> unsigned long Frenchie::Core::Helpers::String::from_string<unsigned long>(const std::string& _Input)
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

template<> unsigned long long Frenchie::Core::Helpers::String::from_string<unsigned long long>(const std::string& _Input)
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

template<> bool Frenchie::Core::Helpers::String::from_string<bool>(const std::string& _Input)
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

template<> std::string Frenchie::Core::Helpers::String::from_string< std::string >(const std::string& _Input)
{
    return _Input;
}

template<> char Frenchie::Core::Helpers::String::from_string<char>(const std::string& _Input)
{
    return _Input.empty() ? ' ' : _Input[0];
}

template<> std::string Frenchie::Core::Helpers::String::to_string<bool>(const bool& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::Helpers::String::to_string<float>(const float& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::Helpers::String::to_string<double>(const double& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::Helpers::String::to_string<long double>(const long double& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::Helpers::String::to_string<short>(const short& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::Helpers::String::to_string<int>(const int& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::Helpers::String::to_string<long>(const long& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::Helpers::String::to_string<long long>(const long long& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::Helpers::String::to_string<unsigned short>(const unsigned short& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::Helpers::String::to_string<unsigned int>(const unsigned int& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::Helpers::String::to_string<unsigned long>(const unsigned long& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::Helpers::String::to_string<unsigned long long>(const unsigned long long& _Input)
{
    return std::to_string(_Input);
}

template<> std::string Frenchie::Core::Helpers::String::to_string<nullptr_t>(const nullptr_t& _Input)
{
    return "0";
}

template<> std::string Frenchie::Core::Helpers::String::to_string<std::string>(const std::string& _Input)
{
    return _Input;
}

template<> std::string Frenchie::Core::Helpers::String::to_string<std::chrono::system_clock::time_point>(const std::chrono::system_clock::time_point& _Input)
{
    std::time_t now = std::chrono::system_clock::to_time_t(_Input);
    std::string time = std::string(std::asctime(std::localtime(&now)));
    time.pop_back();
    return time;
}

// TODO: inplement custom UTF8, UTF16 e.t.c encoders
std::string Frenchie::Core::Helpers::String::as_utf8(const std::wstring& _Input)
{
    return pugi::as_utf8(_Input);
}