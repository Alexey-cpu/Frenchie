#pragma once

// Core
#include <FrenchieCoreISerializer.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>

// Editor
#include <FrenchieEditorLocalThreadQueue.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Configuration
        {
            class Language;
            class Translator;

            struct TranslationUnit final
            {
                mutable std::string Key      = "NEW_KEY";
                mutable std::string Value    = "NEW_VALUE";
                mutable bool        Selected = false;

                struct TransparentComparator
                {
                    using is_transparent = TranslationUnit;

                public:

                    bool operator()(const TranslationUnit& _A, const TranslationUnit& _B) const
                    {
                        return  _A.Key < _B.Key;
                    }

                    bool operator()(const std::string& _A, const TranslationUnit& _B) const
                    {
                        return  _A < _B.Key;
                    }

                    bool operator()(const TranslationUnit& _A, const std::string& _B) const
                    {
                        return  _A.Key < _B;
                    }
                };
            };

            struct TranslationFile final
            {
                mutable std::filesystem::path Path;
                mutable std::set<TranslationUnit, 
                    TranslationUnit::TransparentComparator> Translations;

                typedef std::set<TranslationUnit, 
                    TranslationUnit::TransparentComparator> translations;
            };

            class Language final
            {
            public:
                Language(const std::filesystem::path& _Path, const Translator* _Translator);
                ~Language();

                // getters
                std::string get_name() const;
                std::filesystem::path get_path() const;
                bool is_current() const;

                TranslationFile& get_translation_file() const;

                // API
                void setup();
                void save();

            protected:

                const   std::filesystem::path m_Path   {std::filesystem::path()};
                const   Translator*           m_Owner  {nullptr};
                mutable bool                  m_Current{false};
                mutable TranslationFile       m_TranslationFile;
            };

            class Translator : 
                public Frenchie::Application::Layer, 
                public Frenchie::Core::Serialization::ISerializer
            {
            public:
                Translator();
                virtual ~Translator();

                // getters
                std::filesystem::path get_app_translation_files_path() const;
                std::vector<Frenchie::Core::Reference<Language>> get_supported_languages() const;
                Frenchie::Core::Reference<Language> get_current_language() const;

                // setters
                void set_supported_languages(const std::set<std::filesystem::path>&);

                // Frenchie::Application::Layer
                virtual bool allows_multiple_instances() const override;

                // Frenchie::Core::Serialization::ISerializer
                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

                // API
                Frenchie::Core::Reference<Language> create_new_translation_file(const std::filesystem::path&);

                // static API
                std::string translate(const std::string&);

            protected:

                // friends
                friend class Language;

                // info
                mutable std::filesystem::path                  m_AppTranslationFilesPath;
                mutable std::vector<std::shared_ptr<Language>> m_SupportedLanguages;
                mutable LocalThreadQueue                       m_ThreadsQueue;
            };

            Frenchie::Core::Reference<Frenchie::Application::Configuration::Translator> translator();
        }
    }
}