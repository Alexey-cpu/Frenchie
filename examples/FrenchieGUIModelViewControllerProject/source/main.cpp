#include <FrenchieImmediateUserInterfaceMainWindowController.hpp>
#include <FrenchieFileSystemWatcherLayer.hpp>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    #ifdef ASSETS_PATH
    std::filesystem::path              path(ASSETS_PATH);
    std::vector<std::filesystem::path> paths;

    try
    {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path, std::filesystem::directory_options::skip_permission_denied))
        {
            if(!entry.is_directory() && entry.path().extension().stem() == ".json")
                paths.push_back(entry.path());
        }
    } 
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "File system error: " << e.what() << "\n";
    }

    try
    {
        std::filesystem::path newPath = std::filesystem::current_path().u32string().append(U"/assets/views");
        std::filesystem::create_directories(newPath);
    }
    catch(const std::exception& e)
    {
        std::cerr << "could not create directory " << e.what() << '\n';
    }

    Frenchie::Application::App::push_layer<Frenchie::Application::FileSystemWatcher>(
        paths,
        [](const std::filesystem::path& _File)
        {
            try
            {
                std::cout << "copying file " << _File << "\n";

                std::filesystem::copy_file(
                    _File,
                    std::filesystem::current_path().u32string().append(U"/assets/views/").append(_File.filename().u32string()),
                    std::filesystem::copy_options::overwrite_existing);
            }
            catch(const std::exception& e)
            {
                std::cerr << "could not copy file " << e.what() << '\n';
            }
            
        }
    );

    #endif

    Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceModelViewControllerLayer>(
        "assets/views/FrenchieImmediateUserInterfaceMainWindowView.json",
        std::make_shared<FrenchieImmediateUserInterfaceMainWindowController>());

    return Frenchie::Application::App::execute();
}