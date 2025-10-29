#include <FrenchieEditorFileSystemCreateFolderCommad.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

namespace Frenchie
{
    namespace Editor
    {
        namespace FileSystem
        {
            void create_folder(const std::filesystem::path& _Path)
            {
                if(!std::filesystem::exists(_Path) || !std::filesystem::is_directory(_Path))
                    return;

                auto createdPath = _Path.u32string().append(U"/NewFolder");

                while(std::filesystem::exists(createdPath))
                    createdPath = createdPath.append(U"_Copy");

                try
                {
                    std::filesystem::create_directories(createdPath);
                }
                catch(...){}
            }
        }
    }
}

CreateFolderCommand::CreateFolderCommand(const std::shared_ptr<Frenchie::Application::CommandPayload>& _Payload) : 
    Frenchie::Application::Command::Registry<CreateFolderCommand, const std::shared_ptr<Frenchie::Application::CommandPayload>&>(_Payload){}

CreateFolderCommand::~CreateFolderCommand(){}

void CreateFolderCommand::execute()
{
    auto payload = 
        get_payload<Frenchie::Application::CommandDataPayload<std::filesystem::path>>();

    if(payload != nullptr)
        create_folder(payload->get());
}

std::string CreateFolderCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::Menu::Folder), "Create::Create folder");
}

const bool createFolderCommand = CreateFolderCommand::registerFactory();