#include <FrenchieEditorFileSystemDirectoryTree.hpp>

// Core
#include <FrenchieCoreStringUnicode.hpp>

// Application
#include <FrenchieApplicationCommandsLayer.hpp>

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
                    Frenchie::Application::CommandsQueue::instance()->push(
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
                    Frenchie::Application::CommandsQueue::instance()->push(
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
                    Frenchie::Application::CommandsQueue::instance()->push(
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

    bool opened = ImGui::TreeNodeEx(
            Frenchie::Core::String::convert_utf32_to_utf8(filename.empty() ? U"Root" : filename).c_str(), 
            ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow                                 |
            ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DrawLinesFull                               |
            ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_FramePadding                                |
            (!has_children ? ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf : 0)                  |
            (_Path == m_FocusedPath ? ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Framed : 0)       |
            (m_Selector.contains(_Path) ? ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected : 0) |

            ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_AllowOverlap);

    if(ImGui::IsItemHovered())
    {
        if(ImGui::IsKeyDown(ImGuiKey::ImGuiMod_Ctrl) &&
            ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            m_Selector.select(_Path);
        }
        else if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left)  ||
                ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Middle))
        {
            m_FocusedPath = _Path;
            m_Selector.clear();
        }
    }

    if (ImGui::BeginPopupContextItem())
    {

        Helpers::draw_menu(
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
};