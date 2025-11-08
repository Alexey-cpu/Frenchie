#include <FrenchieEditorFileSystemDirectoryTree.hpp>

// Core
#include <FrenchieCoreFileSystem.hpp>
#include <FrenchieCoreStringUnicode.hpp>
#include <FrenchieCoreStringUtilities.hpp>

// Application
#include <FrenchieApplication.hpp>

// Editor
#include <FrenchieEditorHelpers.hpp>
#include <FrenchieEditorFileSystemCopyPathsCommand.hpp>
#include <FrenchieEditorFileSystemPastePathsCommand.hpp>
#include <FrenchieEditorFileSystemRemovePathsCommand.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>

// FMT
#include <fmt/format.h>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

// add sandbox elements into main menu
namespace Frenchie
{
    namespace Editor
    {
        namespace MainMenu
        {
            class OpenDirectoryTreeViewDebug : 
                public Frenchie::Application::Command::Registry<OpenDirectoryTreeViewDebug, const CommandPayloads&>
            {
            public:

                OpenDirectoryTreeViewDebug(const CommandPayloads& _Sender = CommandPayloads()) : 
                    Frenchie::Application::Command::Registry<OpenDirectoryTreeViewDebug, const CommandPayloads&>(_Sender){}
                virtual ~OpenDirectoryTreeViewDebug(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->push_layer<DirectoryTree>()->show();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Debug::DirectoryTree");
                }
            };

            const bool openDirectoryTreeViewDebug = OpenDirectoryTreeViewDebug::registerFactory();
        }
    }
}

// DirectoryTree
DirectoryTree::DirectoryTree(const std::filesystem::path& _Path) : 
    Frenchie::Application::Layer(STRINGIFY(DirectoryTree)),
    m_RootPath(_Path),
    m_FocusedPath(_Path){}

DirectoryTree::~DirectoryTree(){}

bool DirectoryTree::awake()
{
    // iconify
    default_file = application_platform_backend_renderer()->construct_image(
        std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/appData/themes/icons/default_file.png"));
    
    default_folder = application_platform_backend_renderer()->construct_image(
        std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/appData/themes/icons/default_folder.png"));
    
    default_folder_opened = application_platform_backend_renderer()->construct_image(
        std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/appData/themes/icons/default_folder_opened.png"));

    return true;
}

void DirectoryTree::frame_update()
{
    ImGui::Begin(fmt::format("{}###{}",
        Frenchie::Core::String::convert_utf32_to_utf8(m_RootPath.u32string()),
        get_name()).c_str(),
        &m_Opened);
    {
        ImGui::BeginChild("Tree", ImGui::GetContentRegionAvail());
        {
            if(ImGui::IsWindowHovered())
            {
                // Escape
                if(ImGui::IsKeyPressed(ImGuiKey_Escape))
                    m_Selector.clear();
                
                // Delete
                if(ImGui::IsKeyPressed(ImGuiKey_Delete))
                {
                    Frenchie::Application::application_command_queue()->push(
                        Frenchie::Editor::FileSystem::RemoveFileCommand::factory_id(),
                        {
                            std::make_shared<CommandDataPayload<std::set<std::filesystem::path>>>(m_Selector.get_selected_paths())
                        }
                    );

                    m_Selector.clear();
                }

                // Ctrl + C
                if(ImGui::Shortcut(ImGuiKey::ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_C))
                {
                    Frenchie::Application::application_command_queue()->push(
                        Frenchie::Editor::FileSystem::CopyFileCommand::factory_id(),
                        {
                            std::make_shared<CommandDataPayload<std::set<std::filesystem::path>>>(m_Selector.get_selected_paths())
                        }
                    );

                    m_Selector.clear();
                }

                // Ctrl + V
                if(ImGui::Shortcut(ImGuiKey::ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_V))
                {
                    Frenchie::Application::application_command_queue()->push(
                        Frenchie::Editor::FileSystem::PastePathsCommand::factory_id(),
                        {
                            std::make_shared<CommandDataPayload<std::filesystem::path>>(m_FocusedPath),
                        }
                    );
                }
            }

            if(std::filesystem::exists(m_RootPath))
                draw_paths_tree(m_RootPath);
            ImGui::EndChild();
        }
    }

    ImGui::End();
}

void DirectoryTree::draw_paths_tree(const std::filesystem::path& _Path)
{
    // My code
    if(_Path == m_RootPath.root_path())
        ImGui::SetNextItemOpen(true);

    std::u32string filename = _Path.filename().u32string();

    bool has_children =
        std::filesystem::is_directory(_Path) &&
        std::filesystem::directory_iterator(_Path, std::filesystem::directory_options::skip_permission_denied) != std::filesystem::directory_iterator();

    auto position = ImGui::GetCursorScreenPos();
    auto size     = ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize());

    bool opened = ImGui::TreeNodeEx(
            Frenchie::Core::String::convert_utf32_to_utf8(filename.empty() ? U"Root" : filename).c_str(), 
            //ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DrawLinesFull                               |
            ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_FramePadding                                |
            (!has_children ? ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf : 0)                  |
            ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnDoubleClick                           |
            (m_Selector.contains(_Path) || _Path == m_FocusedPath ? ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected : 0) |
            ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_AllowOverlap);

    auto image = default_file;

    if(std::filesystem::is_directory(_Path))
    {
        if(opened)
            image = default_folder_opened;
        else
            image = default_folder;
    }

    ImGui::GetWindowDrawList()->AddImage(
        image->Ptr, 
        position, 
        position + size,
        ImVec2(1, 0),
        ImVec2(0, 1)
    );
    //

    if(ImGui::IsItemHovered())
    {
        if(ImGui::IsKeyDown(ImGuiKey::ImGuiMod_Ctrl) &&
            ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            m_FocusedPath = _Path;
            m_Selector.select(_Path);
        }
        
        if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left)  ||
                ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Middle))
        {
            m_FocusedPath = _Path;
        }
    }

    if (ImGui::BeginPopupContextItem())
    {
        Helpers::imgui_draw_menu(
            std::filesystem::is_directory(_Path) ?
                STRINGIFY(Frenchie::Editor::FileSystem::Menu::Folder) :
                STRINGIFY(Frenchie::Editor::FileSystem::Menu::File),
            {
                std::make_shared<CommandDataPayload<std::filesystem::path>>(_Path),
                std::make_shared<CommandDataPayload<std::set<std::filesystem::path>>>(m_Selector.get_selected_paths())
            }
        );

        ImGui::EndPopup();
    }

    drag_paths();
    drop_paths_to(_Path);

    if(opened)
    {
        if(std::filesystem::is_directory(_Path))
        {
            for(const auto& directory :
                std::filesystem::directory_iterator(_Path, std::filesystem::directory_options::skip_permission_denied))
            {
                draw_paths_tree(directory.path());
            }
        }

        ImGui::TreePop();
    }
}

void DirectoryTree::drop_paths_to(const std::filesystem::path& _Path)
{
    // drag & drop
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = 
            ImGui::AcceptDragDropPayload(STRINGIFY(std::filesystem::path)))
        {
            auto adress = static_cast<char*>(payload->Data);

            if(!std::filesystem::is_directory(_Path))
                return;

            auto movedPaths = Frenchie::Core::String::split_utf8_string(std::string(adress), "\n");

            for(auto&& movedPath : movedPaths)
            {
                std::filesystem::path oldAdress(Frenchie::Core::String::convert_utf8_to_utf32(movedPath));

                if(!std::filesystem::exists(oldAdress)) 
                    continue;

                std::filesystem::path newAdress(
                    _Path.u32string()
                        .append(U"/")
                        .append(oldAdress.filename().u32string()));

                if(oldAdress.parent_path() == newAdress.parent_path()) 
                    return;

                while(std::filesystem::exists(newAdress))
                {
                    newAdress = 
                        _Path.u32string()
                            .append(U"/")
                            .append(newAdress.filename().stem().u32string())
                            .append(U"_Copy")
                            .append(Frenchie::Core::String::convert_utf8_to_utf32(Frenchie::Core::FileSystem::get_file_extention(oldAdress)));
                }

                try
                {
                    std::filesystem::rename(oldAdress, newAdress);
                }
                catch(...)
                {
                }
            }
        }

        ImGui::EndDragDropTarget();
    }
}

void DirectoryTree::drag_paths()
{
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        std::set<std::filesystem::path> selection;
        auto selectedPaths = m_Selector.get_selected_paths();
        selectedPaths.insert(m_FocusedPath);

        std::string selectionBuffer;

        for(auto&& selectedPath : selectedPaths)
        {
            if(selection.find(selectedPath) != selection.end()) 
                continue;

            selection.insert(selectedPath);

            selectionBuffer.append(Frenchie::Core::String::convert_utf32_to_utf8(selectedPath.u32string())).append("\n");
        }

        ImGui::SetDragDropPayload(STRINGIFY(std::filesystem::path),selectionBuffer.c_str(), selectionBuffer.size() + 1);
        ImGui::TextUnformatted(selectionBuffer.c_str());
        ImGui::EndDragDropSource();
    }
}