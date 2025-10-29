#include <FrenchieCoreFileSystemPastePathsCommand.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

PastePaths::PastePaths(const std::shared_ptr<Frenchie::Application::CommandPayload>& _Sender) : 
    Frenchie::Application::Command::Registry<PastePaths, const std::shared_ptr<Frenchie::Application::CommandPayload>&>(_Sender){}

PastePaths::~PastePaths(){}

void PastePaths::execute()
{
    // TODO: add logic here
}

std::string PastePaths::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem), "Paste");
}