#include <FrenchieApplicationConfigurationLoader.hpp>

// Core
#include <FrenchieCoreFactory.hpp>
#include <FrenchieCoreStringUnicode.hpp>

// Application
#include <FrenchieApplicationCommandsLayer.hpp>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Core;

using namespace Frenchie::Application::Configuration;

ConfigurationLoader::ConfigurationLoader(const std::filesystem::path& _Path) : 
    Layer(STRINGIFY(ConfigurationLoader)), m_Path(_Path){}

ConfigurationLoader::~ConfigurationLoader(){}

std::filesystem::path ConfigurationLoader::get_app_data_path() const
{
    return m_Path;
}

bool ConfigurationLoader::awake()
{
    // push all needed configuration layers into application queue
    for(auto&& creator : Frenchie::Core::Factory::registry())
    {
        if(!Frenchie::Core::String::contains_substring(
            creator.first, 
            STRINGIFY(Frenchie::Application::Configuration))) 
            continue;

        // create new configuration loader by a command
        Frenchie::Application::CommandsQueue::instance()->push(creator.first);
    }

    // push configuration layers deserialization command
    Frenchie::Application::CommandsQueue::instance()->push<CallbackCommand>(
        [this]()
        {
            Frenchie::Core::Serialization::Document document;

            if(!document.read<Frenchie::Core::Serialization::XMLReader>(m_Path)) 
            {
                Frenchie::Core::Logger::instance()->error(
                    fmt::format("{} could not load state", Frenchie::Core::String::convert_utf32_to_utf8(m_Path.u32string())));
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

    document.write<Frenchie::Core::Serialization::XMLBeautifulWriter>(m_Path);
}

bool ConfigurationLoader::allows_multiple_instances() const
{
    return false;
}