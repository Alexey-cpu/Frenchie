#include <FrenchieCoreFileSystemRemovePathsCommand.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

RemoveCommand::RemoveCommand(void* _Sender) : 
    Frenchie::Application::Command::Registry<RemoveCommand, void*>(_Sender){}

RemoveCommand::~RemoveCommand(){}

void RemoveCommand::execute()
{
    // TODO: add logic here
}

std::string RemoveCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem), "Remove");
}