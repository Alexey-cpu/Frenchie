#include <FrenchieApplicationEditorFileSystemExplorerFileMenu.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

// FileSystemExplorerFileMenu
FileSystemExplorerFileMenu::FileSystemExplorerFileMenu(){}
FileSystemExplorerFileMenu::~FileSystemExplorerFileMenu(){}

FileSystemExplorer* FileSystemExplorerFileMenu::get_caller() const
{
    return m_Explorer;
}

void FileSystemExplorerFileMenu::draw(FileSystemExplorer* _Explorer)
{
    // setup who calls
    m_Explorer = _Explorer;

    // draw menu
    MenuDrawer().draw(STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu));
}

bool FileSystemExplorerFileMenu::allows_multiple_instances() const
{
    return false;
}

// FileMenuCopyAction
FileMenuCopyAction::FileMenuCopyAction(){}
FileMenuCopyAction::~FileMenuCopyAction(){}

void FileMenuCopyAction::execute()
{
    auto explorer = Application::instance()->find_or_push<FileSystemExplorerFileMenu>()->get_caller();

    if(explorer != nullptr) 
        explorer->copy_paths();
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
    auto explorer = Application::instance()->find_or_push<FileSystemExplorerFileMenu>()->get_caller();

    if(explorer != nullptr) 
        explorer->paste_paths();
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
    auto explorer = Application::instance()->find_or_push<FileSystemExplorerFileMenu>()->get_caller();

    if(explorer != nullptr) 
        explorer->remove_paths();
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
    auto explorer = Application::instance()->find_or_push<FileSystemExplorerFileMenu>()->get_caller();

    if(explorer != nullptr) 
        explorer->rename_paths();
}

std::string FileMenuRenameAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "rename");
}

FileMenuCreateFolderAction::FileMenuCreateFolderAction(){}
FileMenuCreateFolderAction::~FileMenuCreateFolderAction(){}

void FileMenuCreateFolderAction::execute()
{
    auto explorer = Application::instance()->find_or_push<FileSystemExplorerFileMenu>()->get_caller();

    if(explorer != nullptr) 
        explorer->create_folder();
}

std::string FileMenuCreateFolderAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "create::folder");
}