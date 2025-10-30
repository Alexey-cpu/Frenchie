#include <FrenchieCoreFileSystemPastePathsCommand.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

namespace Frenchie
{
    namespace Editor
    {
        namespace FileSystem
        {
            void paste_paths()
            {
                std::string paths = ImGui::GetClipboardText();
            }
        }
    }
}

PastePaths::PastePaths(const std::shared_ptr<Frenchie::Application::CommandPayload>& _Sender) : 
    Frenchie::Application::Command::Registry<PastePaths, const std::shared_ptr<Frenchie::Application::CommandPayload>&>(_Sender){}

PastePaths::~PastePaths(){}

void PastePaths::execute()
{
}

std::string PastePaths::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem), "Paste");
}