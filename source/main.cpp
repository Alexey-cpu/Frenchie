// //---------------------------------------------------------------------------------------------------
// //Renderer
// //---------------------------------------------------------------------------------------------------
// #include <FrenchieRendererMeshRendererComponent.hpp>
// #include <FrenchieRendererTransformComponent.hpp>
// #include <FrenchieRendererMesh2D.hpp>

// #include <FrenchieApplication.hpp>

// // layers
// #include <FrenchieApplicationCommandsQueueLayer.hpp>
// #include <FrenchieApplicationHierarchyViewLayer.hpp>
// #include <FrenchieApplicationInspectorViewLayer.hpp>
// #include <FrenchieApplicationSceneViewLayer.hpp>
// #include <FrenchieApplicationTimeProviderLayer.hpp>
// #include <FrenchieApplicationCursorWatcherLayer.hpp>

// #include <FrenchieCoreSerializationFormatXML.hpp>
// #include <FrenchieCoreSerializationFormatJSON.hpp>
// #include <FrenchieCoreSerializationFormatYAML.hpp>

// #include <FrenchieCoreSerializationTests.hpp>

// #include <FrenchieCoreChunkAllocator.hpp>

// using namespace Frenchie::Core;
// using namespace Frenchie::Core::Serialization;
// using namespace Frenchie::Renderer;
// using namespace Frenchie::Application;

// class CameraObject : public Object
// {
// public:
//     CameraObject(const std::string& _Name) : Object(_Name)
//     {
//         add_component<Camera>();
//         add_component<Transform>();

//         //std::cout << "CameraObject::CameraObject " << get_name() << "\n";
//     }

//     virtual ~CameraObject()
//     {
//         //std::cout << "CameraObject::~CameraObject " << get_name() << "\n";
//     }

//     virtual void frame_update() override
//     {
//         Object::frame_update();

//         auto camera    = get_component<Camera>();
//         auto transform = get_component<Transform>();

//         if(camera != nullptr && transform != nullptr) 
//             camera->set_position(transform->get_world_position());
//     }
// };

// #include "rapidjson/document.h"
// #include "rapidjson/prettywriter.h"
// #include "rapidjson/stringbuffer.h"
// #include "rapidjson/filewritestream.h"
// #include <rapidjson/writer.h>

// #include <fstream>
// #include <iostream>
// #include <sstream>

// int main(int, char**)
// {
//     // setup logger
//     Frenchie::Core::Logger::instance()->register_sink<spdlog::sinks::stdout_color_sink_mt>();

//     // setup application
//     auto application = Frenchie::Application::Application::instance();
//     application->set_window_size(glm::vec2(2048, 1024));
//     application->set_maximized(true);

//     // push application layers
//     auto mesh = Cache<Rectangle2D>::request("Frenchie/Mesh/Triangle2D");

//     // create shader
//     auto shaderPath = std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared");

//     Logger::instance()->info("---------------------------------------------------------------------------------");
//     Logger::instance()->info(fmt::format("shader path: {}", shaderPath.string()));
//     Logger::instance()->info("---------------------------------------------------------------------------------");

//     auto shader = 
//         Cache<Shader>::request(
//             "Frenchie/Shader/Default",
//             shaderPath.string().append("/shaders/Default/Default.vert"),
//             shaderPath.string().append("/shaders/Default/Default.frag")
//         );

//     // create scene
//     auto scene = std::make_shared<Scene3D>();
//     scene->add_component<Camera>(glm::vec3(+0.f, +0.f, +10000.f), glm::vec3(+0.f, +1.f, +0.f));
//     //scene->add_component<Scene3DBHV>();

//     // create a root object
//     auto root = scene->create_child<CameraObject>(fmt::format("Root"));
//     root->add_component<Transform>();
//     root->add_component<MeshRenderer>(mesh, shader);
//     root->get_component<Transform>()->set_position(glm::vec3(0.f, 0.f, 0.f));

//     //create child objects
//     float radius  = 600.f;
//     int   counter = 0;
//     for(float i = 0; i < 2.f * glm::pi<float>(); i += 2.f * glm::pi<float>() / 10.f)
//     {
//         auto item = root->create_child<CameraObject>(fmt::format("Item-{}", counter++));
//         //item->add_component<Transform>();
//         item->add_component<MeshRenderer>(mesh, shader);
//         item->get_component<Transform>()->set_position(
//             glm::vec3(std::cos(i) * radius, 
//             std::sin(i) * radius, 
//             0.f
//             )
//         );
//     }

//     for(float i = 0; i < 2.f * glm::pi<float>(); i += 2.f * glm::pi<float>() / 10.f)
//     {
//         auto item = root->create_child<CameraObject>(fmt::format("Item-{}", counter++));
//         //item->add_component<Transform>();
//         item->add_component<MeshRenderer>(mesh, shader);
//         item->get_component<Transform>()->set_position(
//             glm::vec3(std::cos(i) * radius, 
//             0.f, 
//             std::sin(i) * radius
//             )
//         );
//     }

//     for(float i = 0; i < 2.f * glm::pi<float>(); i += 2.f * glm::pi<float>() / 10.f)
//     {
//         auto item = root->create_child<CameraObject>(fmt::format("Item-{}", counter++));
//         //item->add_component<Transform>();
//         item->add_component<MeshRenderer>(Cache<Rectangle2D>::request("Frenchie/Mesh/Triangle2D1"), shader);
//         item->get_component<Transform>()->set_position(
//             glm::vec3(std::cos(i) * radius, 
//             400.f, 
//             std::sin(i) * radius
//             )
//         );
//     }

//     // create application layers
//     application->push<CommandsQueue>()->push<CallbackCommand>(
//         [scene]()
//         {
//             if(scene != nullptr) 
//                 scene->awake();
//         }
//     );

//     application->push<TimeProviderLayer>();
//     application->push<CursorWatcher>();

//     application->push<SceneView>("Scene-1", scene);
//     //application->push<SceneView>("Scene-2", scene);
//     application->push<HierarchyView>("Hierarchy", scene);
//     application->push<InspectorView>("Inspector", scene);
//     //application->push<ImguiDemo>();

//     return application->execute();
// }

// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplicationEditorMainWindow.hpp>

int main(int, char**)
{
    return Frenchie::Application::Editor::MainWindow().execute();
}

//---------------------------------------------------------------------------------------------------
// CMake
//---------------------------------------------------------------------------------------------------
// Custom
#include <FrenchieCoreHelpers.hpp>

// STL
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <set>

// FMT
#include <fmt/format.h>

void launch_command(
    const char* _Program, 
    const char* _Arguments = nullptr, 
    const char* _LogFile   = nullptr)
{
    if(_Program == nullptr) 
        return;

    if(_LogFile != nullptr)
    {
        std::system(fmt::format("{} {} > {}", 
            _Program, 
            (_Arguments != nullptr ? _Arguments : " "), 
            _LogFile).c_str());

        return;
    }

    std::system(fmt::format("{} {}", _Program, (_Arguments != nullptr ? _Arguments : " ")).c_str());
}

namespace Frenchie
{
    namespace Core
    {
        namespace CMake
        {
            std::string retrieve_files(
                const std::filesystem::path& _Path,
                const std::set<std::string>& _Formats)
            {
                if(!std::filesystem::exists(_Path) || 
                    _Formats.empty()) 
                    return std::string();

                std::string headerFiles;
                for(auto& directory :
                    std::filesystem::recursive_directory_iterator(
                        _Path,
                        std::filesystem::directory_options::skip_permission_denied))
                {
                    auto path = Frenchie::Core::Helpers::String::replace_symbol(
                        Frenchie::Core::Helpers::String::as_utf8(
                            std::filesystem::relative(directory.path(), _Path.parent_path()).make_preferred().wstring()
                        ),
                        '\\', 
                        '/'
                    );

                    std::cout << std::filesystem::relative(path, _Path.parent_path()) << "\n";

                    if(_Formats.find(Frenchie::Core::Helpers::get_file_extention(path)) != _Formats.end())
                        headerFiles.append("\"").append(path).append("\"\n");
                }

                return headerFiles;
            }

            std::string generate_headers_collection_macro(
                const std::filesystem::path& _Path)
            {
                if(!std::filesystem::exists(_Path)) 
                    return std::string();

                std::string headers = Frenchie::Core::CMake::retrieve_files(_Path, std::set<std::string>({".hpp", ".h"}));
                std::string macro   = "macro(COLLECT_HEADER_FILES)\n";

                if(!headers.empty()) 
                    macro.append(fmt::format("set(HEADERS\n{})\n", headers));
                else 
                    macro.append("set(HEADERS ""\"""\")\n");
                
                macro.append("endmacro()");

                return macro;
            }

            std::string generate_sources_collection_macro(
                const std::filesystem::path& _Path)
            {
                if(!std::filesystem::exists(_Path)) 
                    return std::string();

                std::string headers = Frenchie::Core::CMake::retrieve_files(_Path, std::set<std::string>({".cpp", ".c"}));
                std::string macro   = "macro(COLLECT_SOURCE_FILES)\n";

                if(!headers.empty()) 
                    macro.append(fmt::format("set(SOURCES\n{})\n", headers));
                else 
                    macro.append("set(SOURCES ""\"""\")\n");
                
                macro.append("endmacro()");

                return macro;
            }

            bool create_project(
                const std::filesystem::path& _Path, 
                const std::string&           _ProjectName, 
                const std::string            _ProjectVersion,
                const int                    _CppStandard  = 17)
            {
                // path does not exists
                if(!std::filesystem::exists(_Path))
                    return false;

                // std::filesystem::path
                std::filesystem::path projectPath = 
                    std::filesystem::path(_Path.string()
                        .append("/")
                        .append(_ProjectName))
                        .make_preferred();

                if(std::filesystem::exists(projectPath))
                    return false;

                std::filesystem::create_directories(projectPath);

                // create 'source' folder
                std::filesystem::path sourcePath = std::filesystem::path(projectPath.string().append("/source")).make_preferred();
                std::filesystem::create_directories(sourcePath);
                
                // generate 'main.cpp' file
                std::ofstream main(std::filesystem::path(sourcePath.string().append("/main.cpp")).make_preferred());
                main << 
R"(#include <iostream>
int main(int, char**)
{
    std::cout << "Hello world" << std::endl;
    return 0;
})";
                main.close();

                // create 'cmake' folder
                auto cmakeMacroPath = std::filesystem::path(projectPath.string().append("/cmake")).make_preferred();
                std::filesystem::create_directories(cmakeMacroPath);

                // generate 'headers_collection_macro.cmake'
                std::ofstream headersCollectionMacro(std::filesystem::path(cmakeMacroPath.string().append("/headers_collection_macro.cmake")).make_preferred());
                headersCollectionMacro << generate_headers_collection_macro(sourcePath);
                headersCollectionMacro.close();

                // generate 'sources_collection_macro.cmake'
                std::ofstream sourcesCollectionMacro(std::filesystem::path(cmakeMacroPath.string().append("/sources_collection_macro.cmake")).make_preferred());
                sourcesCollectionMacro << generate_sources_collection_macro(sourcePath);
                sourcesCollectionMacro.close();

                // generate cmake

                // generate 'CMakeLists.txt' file
                std::string cmakeLists;
                cmakeLists.append("# minimum required CMake version \n");
                cmakeLists.append("cmake_minimum_required(VERSION ${CMAKE_VERSION})\n\n");
                cmakeLists.append("# setup project name and version\n");
                cmakeLists.append(fmt::format("project({} VERSION {} LANGUAGES C CXX)\n\n", _ProjectName, _ProjectVersion));
                cmakeLists.append("# setup C/C++ options\n");
                cmakeLists.append("set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)\n");
                cmakeLists.append("set(CMAKE_CXX_STANDARD_REQUIRED ON)\n");
                cmakeLists.append(fmt::format("set(CMAKE_CXX_STANDARD {})\n\n", _CppStandard));
                cmakeLists.append("# optionally include current CMakeLists.txt directory\n");
                cmakeLists.append(
R"(if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif())").append("\n\n");

//                 cmakeLists.append("# optionally include current CMakeLists.txt directory\n");
//                 cmakeLists.append(
// R"(
// cmake_file_api(
//   QUERY
//   API_VERSION 1
// )
// )");

                cmakeLists.append("# include .cmake macro \n");
                cmakeLists.append(fmt::format("include({})\n", """\"cmake/headers_collection_macro.cmake""\""));
                cmakeLists.append(fmt::format("include({})\n\n", """\"cmake/sources_collection_macro.cmake""\""));
                cmakeLists.append("# collect project source code \n");
                cmakeLists.append("cmake_language(CALL COLLECT_SOURCE_FILES)\n");
                cmakeLists.append("cmake_language(CALL COLLECT_HEADER_FILES)\n\n");
                cmakeLists.append("# create executable \n");                
                cmakeLists.append(fmt::format("add_executable({} {} {})", _ProjectName, "${HEADERS}", "${SOURCES}"));

                std::ofstream cmakeListsTxt(std::filesystem::path(projectPath.string().append("/CMakeLists.txt")).make_preferred());
                cmakeListsTxt << cmakeLists;
                cmakeListsTxt.close();

                // generate '.gitignore' file
                std::ofstream dotGitIgnore(std::filesystem::path(projectPath.string().append("/.gitignore")).make_preferred());
                dotGitIgnore <<R"(build/
bin/
dist/
logs/
toolchain.cmake
*.pro.user
*.txt.user
*.ini)";
                dotGitIgnore.close();

                
                return true;
            }
        
        
            bool create_toolchain_file(
                const std::filesystem::path& _Path, 
                const std::filesystem::path& _CCompilePath,
                const std::filesystem::path& _CXXCompilePath)
            {
                // path does not exists
                if(!std::filesystem::exists(_Path))
                    return false;

                // create 'cmake_toolchain' file
                std::string toolchain;
                toolchain.append("# system CMake is compiling for\n");
                toolchain.append("set(CMAKE_SYSTEM ${CMAKE_HOST_SYSTEM})\n");
                toolchain.append("set(CMAKE_SYSTEM_NAME ${CMAKE_HOST_SYSTEM_NAME})\n");
                toolchain.append("set(CMAKE_SYSTEM_VERSION ${CMAKE_HOST_SYSTEM_VERSION})\n");
                toolchain.append("set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_HOST_SYSTEM_PROCESSOR})\n\n");
                toolchain.append("# compilers C/C++ compilers \n");
                toolchain.append(fmt::format("set(CMAKE_C_COMPILER, ""\"{}""\") \n", _CCompilePath.string()));
                toolchain.append(fmt::format("set(CMAKE_CXX_COMPILER, ""\"{}""\") \n", _CXXCompilePath.string()));

                
                std::ofstream file(std::filesystem::path(_Path.string().append("/toolchain.cmake")).make_preferred());
                file << toolchain;
                file.close();

                return true;
            }
        }
    }
}

// #include <float.h>

// int main(int, char**)
// {
//     // std::cout << "PATH: \n";
//     // std::system("echo %PATH%");
//     // std::cout << "\n\n";

//     // std::cout << "LIB: \n";
//     // std::system("echo %LIB%");
//     // std::cout << "\n\n";

//     // std::cout << "INCLUDE: \n";
//     // std::system("echo %INCLUDE%");

//     // auto _Path = std::filesystem::current_path().root_path();
    
//     // for(auto& directory :
//     // std::filesystem::recursive_directory_iterator(
//     //     _Path,
//     //     std::filesystem::directory_options::skip_permission_denied))
//     // {
//     //     std::cout << directory.path() << "\n";
//     // }
    
//     // // create CMake project
//     // std::filesystem::path projectPath = "C:/SDK/Qt_Projects/TestProject";

//     // try
//     // {
//     //     std::filesystem::remove_all(projectPath);
//     // }
//     // catch(const std::exception& e)
//     // {
//     //     std::cerr << e.what() << '\n';
//     // }

//     // Frenchie::Core::CMake::create_project("C:/SDK/Qt_Projects/", "TestProject", "1.0.0");

//     // // create toolchain file for CMake project
//     // std::filesystem::path cCompiler("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe");
//     // std::filesystem::path cppCompiler("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe");

//     // Frenchie::Core::CMake::create_toolchain_file(projectPath, cCompiler, cppCompiler);

//     // // configure project using previously generated CMakeLists.cmake and it's toolchain.cmake file...
//     // std::filesystem::path cmake("C:/Qt/Tools/CMake_64/bin/cmake.exe");
//     // std::filesystem::path build("C:/SDK/Qt_Projects/TestProject/build");
//     // std::filesystem::path cmakeGenerator("C:/Qt/Tools/Ninja/ninja.exe");

//     // std::string arguments;
//     // arguments.append("-DCMAKE_BUILD_TYPE:STRING=Debug").append(" ");
//     // arguments.append("-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE").append(" ");
//     // arguments.append(fmt::format("-DCMAKE_TOOLCHAIN_FILE:FILEPATH={}", projectPath.string().append("/toolchain.cmake"))).append(" ");
//     // arguments.append(fmt::format("-S {}", projectPath.string())).append(" ");
//     // arguments.append(fmt::format("-B {}/build", projectPath.string())).append(" ");
//     // arguments.append("-G Ninja");

//     // launch_command(
//     //     cmake.string().c_str(),
//     //     arguments.c_str(),
//     //     nullptr
//     // );

//     return 0;
// }
//---------------------------------------------------------------------------------------------------