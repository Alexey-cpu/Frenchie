#pragma once

#include <FrenchieApplicationLayer.hpp>

#include <FrenchieApplicationProcessesLayer.hpp>

// STL
#include <set>

namespace Frenchie
{
    namespace Editor
    {
        namespace Tools
        {
            // TranslationFile
            struct TranslationFile
            {
                std::filesystem::path              Path;
                std::map<std::string, std::string> Translations;
            };

            // LoadTranslationFiles
            class LoadTranslationFilesProcess : 
                public Frenchie::Application::Process, 
                public Frenchie::Application::IProcessStatus,
                public Frenchie::Application::IProcessProgress
            {
            public:
                LoadTranslationFilesProcess(
                    const std::set<std::filesystem::path>& _Path,
                    const std::function<void()>&           _OnFinished = nullptr,
                    const std::function<void()>&           _OnCanceled = nullptr, 
                    const std::function<void()>&           _OnFailed   = nullptr);
                
                virtual ~LoadTranslationFilesProcess();

                //
                virtual bool awake() override;

                // Async::IProcessStatus
                virtual std::string iprocess_status_request_status() override;

                // Async::IProcessProgress
                virtual float iprocess_progress_request_progress() override;

                // info
                std::vector<TranslationFile> m_TranslationFiles;
                float                        m_Progress = 0.f;
                std::string                  m_Status   = std::string();
            };

            // SaveTranslationFiles
            class SaveTranslationFilesProcess :
                public Frenchie::Application::Process, 
                public Frenchie::Application::IProcessStatus,
                public Frenchie::Application::IProcessProgress
            {
            public:
                SaveTranslationFilesProcess(
                    const std::vector<TranslationFile>& _Translations,
                    const std::function<void()>&        _OnFinished  = nullptr,
                    const std::function<void()>&        _OnCanceled  = nullptr, 
                    const std::function<void()>&        _OnFailed    = nullptr);
                
                virtual ~SaveTranslationFilesProcess();
            
                virtual bool awake() override;

                // Async::IProcessStatus
                virtual std::string iprocess_status_request_status() override;

                // Async::IProcessProgress
                virtual float iprocess_progress_request_progress() override;

                // info
                std::vector<TranslationFile> m_TranslationFiles;
                float                        m_Progress = 0.f;
                std::string                  m_Status   = std::string();
            };

            // TranslationFilesEditor
            class TranslationFilesEditor : public Frenchie::Application::Layer
            {
            public:
                TranslationFilesEditor();
                virtual ~TranslationFilesEditor();

                virtual void frame_update() override;
                virtual void finish() override;
                virtual bool allows_multiple_instances() const;
            
            protected:
                std::shared_ptr<LoadTranslationFilesProcess> m_LoadProcess;
                std::shared_ptr<SaveTranslationFilesProcess> m_SaveProcess;
                std::mutex                                   m_Mutex;

                bool any_process_is_running();
            };
        }
    }
}