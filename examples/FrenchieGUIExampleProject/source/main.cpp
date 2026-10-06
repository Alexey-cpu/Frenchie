#include <FrenchieImmediateUserInterfaceTestLayer.hpp>
#include <FrenchieFileSystemWatcherLayer.hpp>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    #ifdef ASSETS_PATH
    std::filesystem::path              path(ASSETS_PATH);
    std::vector<std::filesystem::path> paths;

    std::cout << "assets path: " << path.string() << "\n";

    try
    {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path, std::filesystem::directory_options::skip_permission_denied))
        {
            std::cout << entry.path() << "\n";

            if(!entry.is_directory())
                paths.push_back(entry.path());
        }
    } 
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "File system error: " << e.what() << "\n";
    }

    Frenchie::Application::App::push_layer<Frenchie::Application::FileSystemWatcher>(
        paths,
        [](const std::filesystem::path& _File)
        {
            try
            {
                auto thisPath = _File;
                auto newPath  = std::filesystem::path(
                    std::filesystem::current_path().u32string()
                        .append(U"/assets/")
                        .append(_File.parent_path().stem().u32string()));

                if(!std::filesystem::exists(newPath))
                {
                    try
                    {
                        std::filesystem::create_directories(newPath);
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << "could not create directory " << e.what() << '\n';
                    }
                }

                std::filesystem::copy_file(
                    _File,
                    newPath.u32string().append(U"/").append(_File.filename().u32string()),
                    std::filesystem::copy_options::overwrite_existing);
            }
            catch(const std::exception& e)
            {
                std::cerr << "could not copy file " << e.what() << '\n';
            }
            
        }
    );

    #endif

    Frenchie::Application::App::push_layer<Frenchie::Application::FrenchieImmediateUserInterfaceTestLayer>();
    return Frenchie::Application::App::execute();
}