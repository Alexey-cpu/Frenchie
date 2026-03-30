#include <FrenchieApplicationConfigurationLoader.hpp>

// Core
#include <FrenchieCoreFactory.hpp>
#include <FrenchieCoreStringUnicode.hpp>
#include <FrenchieCoreStringUtilities.hpp>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Core;

using namespace Frenchie::Application::Configuration;

ConfigurationLoader::ConfigurationLoader(const std::filesystem::path& _Path) : 
    Layer(STRINGIFY(ConfigurationLoader)), m_Path(_Path){}

ConfigurationLoader::~ConfigurationLoader(){}

std::filesystem::path ConfigurationLoader::get_app_data_directory() const
{
    return m_Path;
}

std::filesystem::path ConfigurationLoader::get_app_state_file_directory() const
{
    auto stateDirectory = 
        std::filesystem::path(get_app_data_directory().wstring().append(L"/state")).make_preferred();

    if(std::filesystem::exists(stateDirectory))
        return stateDirectory;

    try
    {
        std::filesystem::create_directory(stateDirectory);
    }
    catch(...)
    {
        // TODO: put a log here...
    }

    return stateDirectory;
}

std::filesystem::path ConfigurationLoader::get_app_state_file_path()
{
    return std::filesystem::path(get_app_state_file_directory().u32string().append(U"/State.xml")).make_preferred();
}

bool ConfigurationLoader::awake()
{
    // push all needed configuration layers into application queue
    for(auto&& creator : Frenchie::Core::Factory::registry())
    {
        if(!Frenchie::Core::String::utf8_string_contains_substring(
            creator.first, 
            STRINGIFY(Frenchie::Application::Configuration))) 
            continue;

        // create new configuration loader by a command
        Frenchie::Application::application_command_queue()->push(creator.first);
    }

    // push configuration layers deserialization command
    Frenchie::Application::application_command_queue()->push<CallbackCommand>(
        [this]()
        {
            Frenchie::Core::Serialization::Document document;

            if(!document.read<Frenchie::Core::Serialization::XMLReader>(get_app_state_file_path())) 
            {
                // TODO: add log here...
                return;
            }

            auto applicationState = document.find_node(STRINGIFY(Application));

            if(applicationState.is_valid())
            {
                // load .ini file contents
                ImGui::LoadIniSettingsFromMemory(applicationState.get_value());

                for(auto it = Frenchie::Application::application()->begin(); 
                            it != Frenchie::Application::application()->end(); ++it)
                {
                    auto serializer = 
                        std::dynamic_pointer_cast<Frenchie::Core::Serialization::ISerializer>(*it);

                    if(serializer != nullptr) 
                        serializer->deserialize(applicationState);
                }
            }
        }
    );

    return true;
}

void ConfigurationLoader::finish()
{
    Frenchie::Core::Serialization::Document document;

    // append self
    auto applicationState = document.append_node(STRINGIFY(Application), ImGui::SaveIniSettingsToMemory());

    for(auto it = Frenchie::Application::application()->begin(); 
                it != Frenchie::Application::application()->end(); ++it)
    {
        auto serializer = 
            std::dynamic_pointer_cast<Frenchie::Core::Serialization::ISerializer>(*it);

        if(serializer != nullptr) 
            serializer->serialize(applicationState);
    }

    document.write<Frenchie::Core::Serialization::XMLBeautifulWriter>(get_app_state_file_path());
}

bool ConfigurationLoader::allows_multiple_instances() const
{
    return false;
}