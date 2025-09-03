#include <FrenchieApplicationEditorFileSystemExplorerFolderMenu.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

// FileSystemExplorerFolderMenu
FileSystemExplorerFolderMenu::FileSystemExplorerFolderMenu(){}
FileSystemExplorerFolderMenu::~FileSystemExplorerFolderMenu(){}

void FileSystemExplorerFolderMenu::frame_update()
{
    m_MenuDrawer.draw(STRINGIFY(Frenchie::Application::Editor::FileSystem::FolderMenu));
}

// FolderMenuCreateFolderAction
FolderMenuCreateFolderAction::FolderMenuCreateFolderAction(){}
FolderMenuCreateFolderAction::~FolderMenuCreateFolderAction(){}

void FolderMenuCreateFolderAction::execute()
{
    FileSystemExplorer::create_folder();
}

std::string FolderMenuCreateFolderAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FolderMenu), "create::folder");
}

FolderMenuPasteAction::FolderMenuPasteAction(){}
FolderMenuPasteAction::~FolderMenuPasteAction(){}

void FolderMenuPasteAction::execute()
{
    FileSystemExplorer::paste_paths();
}

std::string FolderMenuPasteAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FolderMenu), "paste");
}