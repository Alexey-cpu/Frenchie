#include <FrenchieEditorFileSystemCreateFolderCommad.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

CreateFolderCommand::CreateFolderCommand(void* _Sender) : 
    Frenchie::Application::Command::Registry<CreateFolderCommand, void*>(_Sender){}

CreateFolderCommand::~CreateFolderCommand(){}

void CreateFolderCommand::execute()
{
    // TODO: add logic here
}

std::string CreateFolderCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem), "Create::Create folder");
}