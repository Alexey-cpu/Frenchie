#include <FrenchieApplicationEditorFileSystemExplorerFileMenu.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

// FileSystemExplorerFileMenu
FileSystemExplorerFileMenu::FileSystemExplorerFileMenu(){}
FileSystemExplorerFileMenu::~FileSystemExplorerFileMenu(){}

void FileSystemExplorerFileMenu::draw(FileSystemExplorer* _Explorer)
{
    // draw menu
    Menu().draw(STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), _Explorer);
}

// FileMenuCopyAction
FileMenuCopyAction::FileMenuCopyAction(void* _Sender) : 
    Frenchie::Application::Command::Registry<FileMenuCopyAction, void*>(_Sender){}
FileMenuCopyAction::~FileMenuCopyAction(){}

void FileMenuCopyAction::execute()
{
    get_sender<FileSystemExplorer>()->copy_paths();
}

std::string FileMenuCopyAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "copy");
}

// FileMenuPasteAction
FileMenuPasteAction::FileMenuPasteAction(void* _Sender) : 
    Frenchie::Application::Command::Registry<FileMenuPasteAction, void*>(_Sender){}
FileMenuPasteAction::~FileMenuPasteAction(){}

void FileMenuPasteAction::execute()
{
    get_sender<FileSystemExplorer>()->paste_paths();
}

std::string FileMenuPasteAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "paste");
}

// FileMenuRemoveAction
FileMenuRemoveAction::FileMenuRemoveAction(void* _Sender) : 
    Frenchie::Application::Command::Registry<FileMenuRemoveAction, void*>(_Sender){}
FileMenuRemoveAction::~FileMenuRemoveAction(){}

void FileMenuRemoveAction::execute()
{
    get_sender<FileSystemExplorer>()->remove_paths();
}

std::string FileMenuRemoveAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "remove");
}

// FileMenuRenameAction
FileMenuRenameAction::FileMenuRenameAction(void* _Sender) : 
    Frenchie::Application::Command::Registry<FileMenuRenameAction, void*>(_Sender){}
FileMenuRenameAction::~FileMenuRenameAction(){}

void FileMenuRenameAction::execute()
{
    get_sender<FileSystemExplorer>()->rename_paths();
}

std::string FileMenuRenameAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "rename");
}

FileMenuCreateFolderAction::FileMenuCreateFolderAction(void* _Sender) : 
    Frenchie::Application::Command::Registry<FileMenuCreateFolderAction, void*>(_Sender){}
FileMenuCreateFolderAction::~FileMenuCreateFolderAction(){}

void FileMenuCreateFolderAction::execute()
{
    get_sender<FileSystemExplorer>()->create_folder();
}

std::string FileMenuCreateFolderAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "create::folder");
}