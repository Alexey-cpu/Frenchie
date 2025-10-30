#include <FrenchieCoreFileSystemPastePathsCommand.hpp>

// Core
#include <FrenchieCoreFileSystem.hpp>
#include <FrenchieCoreStringUnicode.hpp>
#include <FrenchieCoreStringUtilities.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

using namespace Frenchie::Core;

using namespace Frenchie::Application;

using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

namespace Frenchie
{
    namespace Editor
    {
        namespace FileSystem
        {
            void paste_paths(const std::filesystem::path& _Path)
            {
                std::vector<std::string> utf8Paths =
                    Frenchie::Core::String::split_utf8_string(ImGui::GetClipboardText(), "\n");

                for(auto&& utf8Path : utf8Paths)
                {
                    std::u32string utf32Path = Frenchie::Core::String::convert_utf8_to_utf32(utf8Path);

                    if(utf32Path.empty())
                        continue;

                    if(!std::filesystem::exists(utf32Path)) 
                    {
                        continue;
                    }

                    auto source    = std::filesystem::path(utf32Path);
                    auto extention = Frenchie::Core::FileSystem::get_file_extention(source);
                    auto target    = std::filesystem::path(
                        _Path.u32string()
                        .append(U"/")
                        .append(source.filename().stem().u32string())
                        .append(U"_Copy")
                        .append(Frenchie::Core::String::convert_utf8_to_utf32(extention))).make_preferred();

                    while(std::filesystem::exists(target))
                    {
                        target = std::filesystem::path(
                            _Path.u32string()
                            .append(U"/")
                            .append(target.filename().stem().u32string())
                            .append(U"_Copy")
                            .append(Frenchie::Core::String::convert_utf8_to_utf32(extention))).make_preferred();
                    }

                    // try to copy
                    try
                    {
                        std::filesystem::copy(source, target);
                    }
                    catch(...)
                    {
                        // TODO: add logic here...
                    }
                }
                
            }
        }
    }
}

PastePaths::PastePaths(const std::shared_ptr<Frenchie::Application::CommandPayload>& _Sender) : 
    Frenchie::Application::Command::Registry<PastePaths, const std::shared_ptr<Frenchie::Application::CommandPayload>&>(_Sender){}

PastePaths::~PastePaths(){}

void PastePaths::execute()
{
    auto pathsPayload = get_payload<CommandDataPayload<std::filesystem::path>>();

    if(pathsPayload != nullptr)
        paste_paths(pathsPayload->get());
}

std::string PastePaths::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::Menu::Folder), "Paste");
}