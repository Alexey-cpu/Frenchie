#pragma once



// Application
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationAsynchronousProcessesLayer.hpp>

// Core
#include <FrenchieCoreISerializer.hpp>

namespace Frenchie
{
    namespace Editor
    {
        namespace Configuration
        {
            class Language;
            class Translator;

            class Language
            {
            public:
                Language(const std::filesystem::path& _Path, const Translator* _Translator);
                ~Language();

                // getters
                std::string get_name() const;
                std::filesystem::path get_path() const;
                bool is_current() const;

                // API
                void setup();

            protected:

                const std::filesystem::path m_Path;
                const Translator*           m_Translator;
                bool                        m_Current   = false;
            };

            struct TranslationUnit
            {
                std::string         Key      = "NEW_KEY";
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

            struct TranslationFile
            {
                mutable std::filesystem::path Path;
                mutable std::set<TranslationUnit, 
                    TranslationUnit::TransparentComparator> Translations;
            };

            class LoadTranslationFilesProcess : 
                public Frenchie::Application::Process, 
                public Frenchie::Application::IProcessStatus,
                public Frenchie::Application::IProcessProgress
            {
            public:
                LoadTranslationFilesProcess(const std::set<std::filesystem::path>& _Path);
                LoadTranslationFilesProcess(const std::filesystem::path& _Path);
                virtual ~LoadTranslationFilesProcess();

                //
                virtual bool awake() override;

                // Async::IProcessStatus
                virtual std::string iprocess_status_request_status() override;

                // Async::IProcessProgress
                virtual float iprocess_progress_request_progress() override;

                // info
                std::vector<TranslationFile> m_TranslationFiles;

            protected:
                float                        m_Progress = 0.f;
                std::string                  m_Status   = std::string();
            };

            class SaveTranslationFilesProcess :
                public Frenchie::Application::Process, 
                public Frenchie::Application::IProcessStatus,
                public Frenchie::Application::IProcessProgress
            {
            public:
                SaveTranslationFilesProcess(const std::vector<TranslationFile>& _Translations);
                virtual ~SaveTranslationFilesProcess();
            
                virtual bool awake() override;

                // Async::IProcessStatus
                virtual std::string iprocess_status_request_status() override;

                // Async::IProcessProgress
                virtual float iprocess_progress_request_progress() override;

                // info
                std::vector<TranslationFile> m_TranslationFiles;

            protected:
                float       m_Progress = 0.f;
                std::string m_Status   = std::string();
            };

            class Translator : 
                public Frenchie::Application::Layer, 
                public Frenchie::Core::Serialization::ISerializer
            {
            public:
                Translator();
                virtual ~Translator();

                // getters
                std::vector<std::unique_ptr<Language>>& get_supported_languages() const;
                void set_supported_languages(const std::set<std::filesystem::path>&);

                // Frenchie::Application::Layer
                virtual bool allows_multiple_instances() const override;

                // Frenchie::Core::Serialization::ISerializer
                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

                // static API
                static std::string translate(const std::string&);
                static Frenchie::Core::Reference<Translator> instance();

            protected:

                // friends
                friend class Language;

                // info
                mutable TranslationFile                              m_TranslationFile;
                mutable std::vector<std::unique_ptr<Language>>       m_SupportedLanguages;
                mutable std::shared_ptr<LoadTranslationFilesProcess> m_Process;
            };
        }
    }
}