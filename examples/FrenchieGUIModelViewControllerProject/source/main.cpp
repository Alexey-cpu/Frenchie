#include <FrenchieImmediateUserInterfaceMainWindowController.hpp>

class ViewFilesWatcherLayer : public Frenchie::Application::Layer
{
public:
    ViewFilesWatcherLayer(
        const std::vector<std::filesystem::path>&               _Paths,
        const std::function<void(const std::filesystem::path&)> _Callback) :
        Frenchie::Application::Layer(STRINGIFY(ViewFilesWatcherLayer)),
        m_Callback(_Callback)
    {
        for(auto& path : _Paths)
        {
            if(!std::filesystem::exists(path) || std::filesystem::is_directory(path))
                continue;

            m_Files[path] = std::filesystem::file_time_type();
        }
    }

    virtual ~ViewFilesWatcherLayer(){}

    virtual bool awake()
    {
        process_files();
        return !m_Files.empty();
    }

    virtual void frame_start() override
    {
        process_files();
    }

private:

    void process_files()
    {
        for(auto file : m_Files)
        {
            if(!std::filesystem::exists(file.first) || std::filesystem::is_directory(file.first) || file.second == std::filesystem::last_write_time(file.first))
                continue;

            if(m_Callback != nullptr)
                m_Callback(file.first);

            m_Files[file.first] = std::filesystem::last_write_time(file.first);
        }
    }

    std::map<std::filesystem::path, std::filesystem::file_time_type> m_Files;
    std::function<void(const std::filesystem::path&)>                m_Callback;
};

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    #ifdef ASSETS_PATH
        std::cout << "ASSETS_PATH " << ASSETS_PATH << "\n";

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

    Frenchie::Application::App::push_layer<ViewFilesWatcherLayer>(
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