// Application
#include <FrenchieFileSystemWatcherLayer.hpp>

// Core
#include <FrenchieCoreStringUtilities.hpp>

using namespace Frenchie::Application;

FileSystemWatcher::FileSystemWatcher(
    const std::vector<std::filesystem::path>&               _Paths,
    const std::function<void(const std::filesystem::path&)> _Callback) :
    Frenchie::Application::Layer(STRINGIFY(FileSystemWatcher)),
    m_Callback(_Callback)
{
    for(auto& path : _Paths)
    {
        if(std::filesystem::exists(path))
            m_Files[path] = std::filesystem::file_time_type();
    }
}

FileSystemWatcher::~FileSystemWatcher(){}

bool FileSystemWatcher::awake()
{
    bool awakened = !m_Files.empty();

    if(awakened)
        process_paths();

    return awakened;
}

void FileSystemWatcher::frame_start()
{
    process_paths();
}

void FileSystemWatcher::process_paths()
{
    for(auto file : m_Files)
    {
        if(!std::filesystem::exists(file.first) || file.second == std::filesystem::last_write_time(file.first))
            continue;

        if(m_Callback != nullptr)
            m_Callback(file.first);

        m_Files[file.first] = std::filesystem::last_write_time(file.first);
    }
}