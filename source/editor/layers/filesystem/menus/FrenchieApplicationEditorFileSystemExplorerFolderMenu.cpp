#include <FrenchieApplicationEditorFileSystemExplorerFolderMenu.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

// FileSystemExplorerFolderMenu
FileSystemExplorerFolderMenu::FileSystemExplorerFolderMenu(){}
FileSystemExplorerFolderMenu::~FileSystemExplorerFolderMenu(){}

void FileSystemExplorerFolderMenu::draw(FileSystemExplorer* _Explorer)
{
    Menu().draw(STRINGIFY(Frenchie::Application::Editor::FileSystem::FolderMenu), _Explorer);
}

// FolderMenuCreateFolderAction
FolderMenuCreateFolderAction::FolderMenuCreateFolderAction(void* _Sender) : 
    Frenchie::Application::Command::Registry<FolderMenuCreateFolderAction, void*>(_Sender){}
FolderMenuCreateFolderAction::~FolderMenuCreateFolderAction(){}

void FolderMenuCreateFolderAction::execute()
{
    get_sender<FileSystemExplorer>()->create_folder();
}

std::string FolderMenuCreateFolderAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FolderMenu), "create::folder");
}

FolderMenuPasteAction::FolderMenuPasteAction(void* _Sender) : 
    Frenchie::Application::Command::Registry<FolderMenuPasteAction, void*>(_Sender){}
FolderMenuPasteAction::~FolderMenuPasteAction(){}

void FolderMenuPasteAction::execute()
{
    get_sender<FileSystemExplorer>()->paste_paths();
}

std::string FolderMenuPasteAction::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FolderMenu), "paste");
}