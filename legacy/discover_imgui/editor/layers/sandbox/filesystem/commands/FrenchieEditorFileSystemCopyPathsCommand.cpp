#include <FrenchieEditorFileSystemCopyPathsCommand.hpp>

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
CopyFileCommand::CopyFileCommand(const CommandPayloads& _Sender) : 
    Frenchie::Application::Command::Registry<CopyFileCommand, const CommandPayloads&>(_Sender){}

CopyFileCommand::~CopyFileCommand(){}

void CopyFileCommand::execute()
{
    std::set<std::filesystem::path> copy;

    auto pathPayload  = get_payload<CommandDataPayload<std::filesystem::path>>();
    auto pathsPayload = get_payload<CommandDataPayload<std::set<std::filesystem::path>>>();

    if(pathPayload != nullptr)
        copy.insert(pathPayload->get());

    if(pathsPayload != nullptr)
    {
        auto paths = pathsPayload->get();

        for (auto&& path : paths)
            copy.insert(path);
    }

    if(!copy.empty())
        copy_paths(copy);
}

std::string CopyFileCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::Menu::File), "Copy");
}

// CopyFolderCommand
CopyFolderCommand::CopyFolderCommand(const CommandPayloads& _Sender) : 
    Frenchie::Application::Command::Registry<CopyFolderCommand, const CommandPayloads&>(_Sender){}

CopyFolderCommand::~CopyFolderCommand(){}

void CopyFolderCommand::execute()
{
    std::set<std::filesystem::path> copy;

    auto pathPayload  = get_payload<CommandDataPayload<std::filesystem::path>>();
    auto pathsPayload = get_payload<CommandDataPayload<std::set<std::filesystem::path>>>();

    if(pathPayload != nullptr)
        copy.insert(pathPayload->get());

    if(pathsPayload != nullptr)
    {
        auto paths = pathsPayload->get();

        for (auto&& path : paths)
            copy.insert(path);
    }

    if(!copy.empty())
        copy_paths(copy);
}

std::string CopyFolderCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::Menu::Folder), "Copy");
}

const bool FrenchieEditorFileSystemCopyFileCommand   = CopyFileCommand::Registry::registerFactory();
const bool FrenchieEditorFileSystemCopyFolderCommand = CopyFolderCommand::Registry::registerFactory();