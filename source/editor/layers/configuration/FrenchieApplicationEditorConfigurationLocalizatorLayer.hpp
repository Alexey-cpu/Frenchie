#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreISerializer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            namespace Configuration
            {
                class Localizator : 
                    public Layer, 
                    public Frenchie::Core::Serialization::ISerializer
                {
                public:
                    Localizator();
                    virtual ~Localizator();

                    // Frenchie::Application::Layer
                    virtual bool allows_multiple_instances() const override;

                    // Frenchie::Core::Serialization::ISerializer
                    virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                    virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

                    // static API
                    static std::string translation(const std::string&);
                    static void setup_translation_files_location(const std::filesystem::path&);
                    static void setup_language(const std::string&);

                protected:

                    mutable std::filesystem::path m_Path = 
                        std::filesystem::current_path();

                    mutable std::map<
                        std::string, 
                        std::string> m_Translations;

                    mutable std::map<std::string, bool> m_Languages = 
                    {
                        {"Russian", false},
                        {"English", true}
                    };
                };
            }
        }
    }
}