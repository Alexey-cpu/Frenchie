#include <FrenchieEditorFileSystemDirectoryTree.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

// Core
#include <FrenchieCoreStringUnicode.hpp>

// Application
#include <FrenchieApplicationCommandsLayer.hpp>

// Editor
#include <FrenchieEditorHelpers.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>

// FMT
#include <fmt/format.h>

// add sandbox elements into main menu
namespace Frenchie
{
    namespace Editor
    {
        namespace MainMenu
        {
            class OpenDirectoryTreeViewDebug : 
                public Frenchie::Application::Command::Registry<OpenDirectoryTreeViewDebug, const std::shared_ptr<CommandPayload>&>
            {
            public:

                OpenDirectoryTreeViewDebug(const std::shared_ptr<CommandPayload>& _Sender = nullptr) : 
                    Frenchie::Application::Command::Registry<OpenDirectoryTreeViewDebug, const std::shared_ptr<CommandPayload>&>(_Sender){}
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
    m_Path(_Path){}

DirectoryTree::~DirectoryTree(){}

void DirectoryTree::frame_update()
{
    ImGui::Begin(fmt::format("{}###{}",
        get_name(),
        Frenchie::Core::String::convert_utf32_to_utf8(m_Path.u32string())).c_str(),
        &m_Opened);
    {
        ImGui::BeginChild("Tree", ImGui::GetContentRegionAvail());
        {
            if(std::filesystem::exists(m_Path))
                draw_paths_tree(m_Path);
            ImGui::EndChild();
        }
    }

    ImGui::End();
}

void DirectoryTree::draw_paths_tree(const std::filesystem::path& _Path)
{
    if(_Path == m_Path.root_path())
        ImGui::SetNextItemOpen(true);

    std::u32string filename = _Path.filename().u32string();

    bool opened = ImGui::TreeNodeEx(
            Frenchie::Core::String::convert_utf32_to_utf8(filename.empty() ? U"Root" : filename).c_str(), 
            ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow       |
            ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DrawLinesFull     |
            ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnDoubleClick |
            (m_Selector.contains(_Path) ? ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Framed : 0)|
            ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_AllowOverlap);

    if(ImGui::IsItemHovered() && 
        ImGui::Shortcut(ImGuiKey::ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_MouseLeft))
    {
        m_Selector.select(_Path);
    }

    if (ImGui::BeginPopupContextItem())
    {
        if(m_Selector.empty())
        {
            Helpers::draw_menu(
                std::filesystem::is_directory(_Path) ?
                    STRINGIFY(Frenchie::Editor::FileSystem::Menu::Folder) :
                    STRINGIFY(Frenchie::Editor::FileSystem::Menu::File),
                std::make_shared<CommandDataPayload<std::filesystem::path>>(_Path));
        }
        else
        {
            // TODO: add logic here...
        }

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