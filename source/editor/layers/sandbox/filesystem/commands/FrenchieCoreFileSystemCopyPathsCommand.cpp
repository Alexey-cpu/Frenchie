#include <FrenchieCoreFileSystemCopyPathsCommand.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

CopyPaths::CopyPaths(void* _Sender) : 
    Frenchie::Application::Command::Registry<CopyPaths, void*>(_Sender){}

CopyPaths::~CopyPaths(){}

void CopyPaths::execute()
{
    // TODO: add logic here
}

std::string CopyPaths::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem), "Copy");
}