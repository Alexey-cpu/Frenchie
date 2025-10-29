#include <FrenchieCoreFileSystemCopyPathsCommand.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

CopyPaths::CopyPaths(const std::shared_ptr<Frenchie::Application::CommandPayload>& _Sender) : 
    Frenchie::Application::Command::Registry<CopyPaths, const std::shared_ptr<Frenchie::Application::CommandPayload>&>(_Sender){}

CopyPaths::~CopyPaths(){}

void CopyPaths::execute()
{
    // TODO: add logic here
}

std::string CopyPaths::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem), "Copy");
}