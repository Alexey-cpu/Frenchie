#include <FrenchieEditorFileSystemCreateFileCommand.hpp>

// STL
#include <iostream> // For console output (optional)
#include <fstream>  // Required for file operations
#include <string>   // For using std::string

using namespace Frenchie::Core;
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

namespace Frenchie
{
    namespace Editor
    {
        namespace FileSystem
        {
            void create_file(const std::filesystem::path& _Path)
            {
                if(!std::filesystem::exists(_Path) || !std::filesystem::is_directory(_Path))
                    return;

                auto createdPath = _Path.u32string().append(U"/NewFile");

                while(std::filesystem::exists(createdPath))
                    createdPath = createdPath.append(U"_Copy");

                // save file
                try
                {
                    std::ofstream outputFile;
                    outputFile.open(std::filesystem::path(createdPath));
                    outputFile.close();
                }
                catch(...){}
            }
        }
    }
}

CreateFileCommand::CreateFileCommand(const CommandPayloads& _Sender) : 
    Frenchie::Application::Command::Registry<CreateFileCommand, const CommandPayloads&>(_Sender){}

CreateFileCommand::~CreateFileCommand(){}

void CreateFileCommand::execute()
{
    auto payload = 
        get_payload<Frenchie::Application::CommandDataPayload<std::filesystem::path>>();

    if(payload != nullptr)
        create_file(payload->get());
}

std::string CreateFileCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::Menu::Folder), "Create::Create file");
}

const bool FrenchieEditorCreateFileCommand = CreateFileCommand::registerFactory();