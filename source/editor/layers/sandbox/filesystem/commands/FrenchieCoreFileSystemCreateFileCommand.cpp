#include <FrenchieCoreFileSystemCreateFileCommand.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

CreateFileCommand::CreateFileCommand(const std::shared_ptr<Frenchie::Application::CommandPayload>& _Sender) : 
    Frenchie::Application::Command::Registry<CreateFileCommand, const std::shared_ptr<Frenchie::Application::CommandPayload>&>(_Sender){}

CreateFileCommand::~CreateFileCommand(){}

void CreateFileCommand::execute()
{
    // TODO: add logic here
}

std::string CreateFileCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem), "Create::Create file");
}