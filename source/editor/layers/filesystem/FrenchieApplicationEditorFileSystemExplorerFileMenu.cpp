#include <FrenchieApplicationEditorFileSystemExplorerFileMenu.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

// FileSystemExplorerFileMenu
FileSystemExplorerFileMenu::FileSystemExplorerFileMenu(){}
FileSystemExplorerFileMenu::~FileSystemExplorerFileMenu(){}

void FileSystemExplorerFileMenu::frame_update()
{
    m_MenuDrawer.draw(STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu));
}

// FileMenuCopyAction
FileMenuCopyAction::FileMenuCopyAction(){}
FileMenuCopyAction::~FileMenuCopyAction(){}

void FileMenuCopyAction::execute()
{
    FileSystemExplorer::copy_paths();
}

std::string FileMenuCopyAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "copy");
}

// FileMenuPasteAction
FileMenuPasteAction::FileMenuPasteAction(){}
FileMenuPasteAction::~FileMenuPasteAction(){}

void FileMenuPasteAction::execute()
{
    FileSystemExplorer::paste_paths();
}

std::string FileMenuPasteAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "paste");
}

// FileMenuRemoveAction
FileMenuRemoveAction::FileMenuRemoveAction(){}
FileMenuRemoveAction::~FileMenuRemoveAction(){}

void FileMenuRemoveAction::execute()
{
    FileSystemExplorer::remove_paths();
}

std::string FileMenuRemoveAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "remove");
}

// FileMenuRenameAction
FileMenuRenameAction::FileMenuRenameAction(){}
FileMenuRenameAction::~FileMenuRenameAction(){}

void FileMenuRenameAction::execute()
{
    FileSystemExplorer::rename_paths();
}

std::string FileMenuRenameAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "rename");
}

FileMenuCreateFolderAction::FileMenuCreateFolderAction(){}
FileMenuCreateFolderAction::~FileMenuCreateFolderAction(){}

void FileMenuCreateFolderAction::execute()
{
    FileSystemExplorer::create_folder();
}

std::string FileMenuCreateFolderAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "create::folder");
}