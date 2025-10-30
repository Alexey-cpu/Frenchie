#include <FrenchieCoreFileSystemCopyPathsCommand.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

// Core
#include <FrenchieCoreStringUnicode.hpp>

// STL

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
            static void copy_paths(const std::set<std::filesystem::path>& _Paths)
            {
                std::u32string paths;
                for(auto path : _Paths)
                    paths.append(path.u32string()).append(U"\n");

                ImGui::SetClipboardText(
                    Frenchie::Core::String::convert_utf32_to_utf8(paths).c_str());
            }
        }
    }
}

// CopyFileCommand
CopyFileCommand::CopyFileCommand(const std::shared_ptr<Frenchie::Application::CommandPayload>& _Sender) : 
    Frenchie::Application::Command::Registry<CopyFileCommand, const std::shared_ptr<Frenchie::Application::CommandPayload>&>(_Sender){}

CopyFileCommand::~CopyFileCommand(){}

void CopyFileCommand::execute()
{
    auto pathPayload =get_payload<CommandDataPayload<std::filesystem::path>>();

    if(pathPayload != nullptr)
        copy_paths(std::set<std::filesystem::path>({pathPayload->get()}));
}

std::string CopyFileCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::Menu::File), "Copy");
}

// CopyFolderCommand
CopyFolderCommand::CopyFolderCommand(const std::shared_ptr<Frenchie::Application::CommandPayload>& _Sender) : 
    Frenchie::Application::Command::Registry<CopyFolderCommand, const std::shared_ptr<Frenchie::Application::CommandPayload>&>(_Sender){}

CopyFolderCommand::~CopyFolderCommand(){}

void CopyFolderCommand::execute()
{
    auto pathPayload = get_payload<CommandDataPayload<std::filesystem::path>>();

    if(pathPayload != nullptr)
        copy_paths(std::set<std::filesystem::path>({pathPayload->get()}));
}

std::string CopyFolderCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::Menu::Folder), "Copy");
}

// CopySelectionCommand
CopySelectionCommand::CopySelectionCommand(const std::shared_ptr<Frenchie::Application::CommandPayload>& _Sender) : 
    Frenchie::Application::Command::Registry<CopySelectionCommand, const std::shared_ptr<Frenchie::Application::CommandPayload>&>(_Sender){}

CopySelectionCommand::~CopySelectionCommand(){}

void CopySelectionCommand::execute()
{
    auto pathsPayload = get_payload<CommandDataPayload<std::set<std::filesystem::path>>>();

    if(pathsPayload != nullptr)
        copy_paths(pathsPayload->get());
}

std::string CopySelectionCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::Menu::Selection), "Copy");
}

const bool FrenchieEditorFileSystemCopyFileCommand      = CopyFileCommand::Registry::registerFactory();
const bool FrenchieEditorFileSystemCopyFolderCommand    = CopyFileCommand::Registry::registerFactory();
const bool FrenchieEditorFileSystemCopySelectionCommand = CopyFileCommand::Registry::registerFactory();