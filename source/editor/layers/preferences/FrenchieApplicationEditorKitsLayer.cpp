// #include <FrenchieApplicationEditorKitsLayer.hpp>

// // Core
// #include <FrenchieCoreHelpers.hpp>

// // IMGUI
// #include <imgui.h>

// using namespace Frenchie::Application;
// using namespace Frenchie::Application::Editor;

// Kits::Kits() : Frenchie::Application::Layer::Registry<Kits>(STRINGIFY(Kits)){}
// Kits::~Kits(){}

// bool Kits::awake()
// {
//     // read system environment variables
//     #ifdef _WIN32
//         auto paths    = Frenchie::Core::String::split(Frenchie::Core::CommandLine::execute_command(fmt::format("echo %{}%", "PATH")).Message, ";");
//         auto libs     = Frenchie::Core::String::split(Frenchie::Core::CommandLine::execute_command(fmt::format("echo %{}%", "LIB")).Message, ";");
//         auto includes = Frenchie::Core::String::split(Frenchie::Core::CommandLine::execute_command(fmt::format("echo %{}%", "INCLUDE")).Message, ";");

//         for(auto&& path : paths)
//             m_PATH.insert(std::filesystem::path(path).make_preferred());

//         for(auto&& lib : libs)
//             m_LIB.insert(std::filesystem::path(lib).make_preferred());

//         for(auto&& include : includes)
//             m_INCLUDE.insert(std::filesystem::path(include).make_preferred());
//     #else
//     #endif

//     for(auto&& path : m_PATH) Frenchie::Core::Logger::instance()->critical(path.string());

//     // CMakes
//     {
//         m_CMakes = Kits::scan_for_package(m_PATH, "cmake");
//     }

//     // C compilers
//     {
//         std::vector<std::string> ccompilers = {"gcc", "cl", "clang"};

//         for(auto&& ccompiler : ccompilers)
//         {
//             auto compiler = Kits::scan_for_package(m_PATH, ccompiler);
//             m_CCompilers.insert(compiler.begin(), compiler.end());
//         }
//     }

//     // CXXCompilers
//     {
//         std::vector<std::string> ccompilers = {"g++", "cl", "clang++"};

//         for(auto&& ccompiler : ccompilers)
//         {
//             auto compiler = Kits::scan_for_package(m_PATH, ccompiler);
//             m_CXXCompilers.insert(compiler.begin(), compiler.end());
//         }
//     }

//     // Generators
//     {
//         m_Generators = Kits::scan_for_package(m_PATH, "ninja");
//     }

//     return Layer::awake();
// }

// void Kits::frame_update()
// {
//     ImGui::BeginChild(get_name().c_str());
//     {
//         Kits::draw_package(m_CMakes, "CMake");
//         Kits::draw_package(m_CCompilers, "C compilers");
//         Kits::draw_package(m_CXXCompilers, "C++ compilers");
//         Kits::draw_package(m_Generators, "Generators");
//     }

//     ImGui::EndChild();
// }

// bool Kits::allows_multiple_instances() const
// {
//     return false;
// }

// std::string Kits::factory_id()
// {
//     return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::Preferences), STRINGIFY(Kits));
// }

// std::map<std::filesystem::path, bool> Kits::scan_for_package(
//     const std::set<std::filesystem::path>& _Paths, 
//     const std::string&                     _Package)
// {
//     std::map<std::filesystem::path, bool> package;

//     for(auto&& path : _Paths)
//     {
//         if(Frenchie::Core::String::contains_substring(Frenchie::Core::String::to_lower(path.string()), _Package) || 
//             Frenchie::Core::String::contains_substring(Frenchie::Core::String::to_lower(path.string()), "bin") || 
//             Frenchie::Core::String::contains_substring(Frenchie::Core::String::to_lower(path.string()), "qt"))
//         {
//             try
//             {
//                 for(auto&& directory : 
//                     std::filesystem::recursive_directory_iterator(path, 
//                         std::filesystem::directory_options::skip_permission_denied))
//                 {
//                     if(!directory.is_directory() && 
//                         Frenchie::Core::String::to_lower(directory.path().filename().stem().string()) == _Package) 
//                     {
//                         package.insert(
//                             {
//                                 directory.path(), 
//                                 false
//                             }
//                         );
//                     }
//                 }
//             }
//             catch(const std::exception& e)
//             {
//                 Frenchie::Core::Logger::instance()->critical(e.what());
//             }
//         }
//     }

//     return package;
// }

// void Kits::draw_package(
//     std::map<std::filesystem::path, bool>& _Package, 
//     const std::string& _Name)
// {
//     if(ImGui::TreeNode(_Name.c_str()))
//     {
//         for(auto&& cmake : _Package)
//         {
//             auto path     = cmake.first;
//             auto selected = cmake.second;

//             if(ImGui::Selectable(
//                 Frenchie::Core::String::as_utf8(cmake.first.wstring()).c_str(), 
//                 &cmake.second,
//                 ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns    | 
//                 ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowOverlap      | 
//                 ImGuiSelectableFlags_::ImGuiSelectableFlags_NoAutoClosePopups |
//                 ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowDoubleClick))
//             {
//                 for(auto&& cmake : _Package) 
//                     cmake.second = false;

//                 cmake.second = true;
//             }
//         }

//         ImGui::TreePop();
//     }
// }