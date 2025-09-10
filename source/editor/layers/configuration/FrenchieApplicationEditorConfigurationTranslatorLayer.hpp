#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreISerializer.hpp>

namespace Frenchie
{
    namespace Editor
    {
        namespace Configuration
        {
            class Translator : 
                public Frenchie::Application::Layer, 
                public Frenchie::Core::Serialization::ISerializer
            {
            public:
                Translator();
                virtual ~Translator();

                // getters
                std::map<std::string, std::string>& get_translations() const;
                std::set<std::string>& get_supported_languages() const;
                std::string get_current_language() const;

                // setters
                void set_translation_files_path(const std::filesystem::path& _Path);
                void set_language(const std::string&);

                // Frenchie::Application::Layer
                virtual bool allows_multiple_instances() const override;

                // Frenchie::Core::Serialization::ISerializer
                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

                // static API
                static std::string translate(const std::string&);

            protected:

                // info
                mutable std::filesystem::path              m_Path               = std::filesystem::path();
                mutable std::map<std::string, std::string> m_Translations       = std::map<std::string, std::string>();
                mutable std::string                        m_CurrentLanguage    = "English";
                mutable std::set<std::string>              m_SupportedLanguages = {"Russian","English"};

                // service methods
                std::filesystem::path translation_file_path();
            };
        }
    }
}