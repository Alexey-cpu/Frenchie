#include <FrenchieCoreFileSystem.hpp>

#include <FrenchieCorePlatform.hpp>
#include <FrenchieCoreStringUnicode.hpp>

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

using namespace Frenchie::Core::FileSystem;

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

FILE* Frenchie::Core::FileSystem::open_file(const std::u32string& _Path, std::u32string _Mode)
{
    return std::fopen(
        Frenchie::Core::String::convert_utf32_to_utf8(_Path).c_str(),
        Frenchie::Core::String::convert_utf32_to_utf8(_Mode).c_str());
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