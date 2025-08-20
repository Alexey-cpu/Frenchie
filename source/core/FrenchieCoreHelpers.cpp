#include <FrenchieCoreHelpers.hpp>

using namespace Frenchie::Core::Helpers;

#ifdef _MSC_VER
#include <windows.h>
#include <libloaderapi.h>
#endif

//Returns the absolute path of the executable
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