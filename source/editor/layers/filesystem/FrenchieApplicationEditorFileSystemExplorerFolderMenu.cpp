#include <FrenchieApplicationEditorFileSystemExplorerFolderMenu.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

// FileSystemExplorerFolderMenu
FileSystemExplorerFolderMenu::FileSystemExplorerFolderMenu(){}
FileSystemExplorerFolderMenu::~FileSystemExplorerFolderMenu(){}

FileSystemExplorer* FileSystemExplorerFolderMenu::get_caller() const
{
    return m_Explorer;
}

void FileSystemExplorerFolderMenu::draw(FileSystemExplorer* _Explorer)
{
    // setup who calls
    m_Explorer = _Explorer;

    // draw menu
    MenuDrawer().draw(STRINGIFY(Frenchie::Application::Editor::FileSystem::FolderMenu));
}

bool FileSystemExplorerFolderMenu::allows_multiple_instances() const
{
    return false;
}

// FolderMenuCreateFolderAction
FolderMenuCreateFolderAction::FolderMenuCreateFolderAction(){}
FolderMenuCreateFolderAction::~FolderMenuCreateFolderAction(){}

void FolderMenuCreateFolderAction::execute()
{
    auto explorer = Application::instance()->find_or_push<FileSystemExplorerFolderMenu>()->get_caller();

    if(explorer != nullptr) 
        explorer->create_folder();
}

std::string FolderMenuCreateFolderAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FolderMenu), "create::folder");
}

FolderMenuPasteAction::FolderMenuPasteAction(){}
FolderMenuPasteAction::~FolderMenuPasteAction(){}

void FolderMenuPasteAction::execute()
{
    auto explorer = Application::instance()->find_or_push<FileSystemExplorerFolderMenu>()->get_caller();

    if(explorer != nullptr) 
        explorer->paste_paths();
}

std::string FolderMenuPasteAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FolderMenu), "paste");
}