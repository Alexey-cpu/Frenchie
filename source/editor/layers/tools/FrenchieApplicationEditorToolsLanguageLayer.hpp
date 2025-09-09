#pragma once

#include <FrenchieApplicationLayer.hpp>

#include <FrenchieApplicationEditorAsyncProcessLayer.hpp>

// STL
#include <set>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            namespace Async
            {
                // LoadTranslationFiles
                class LoadTranslationFilesProcess : 
                    public Async::Process, 
                    public Async::IProcessStatus,
                    public Async::IProcessProgress
                {
                public:
                    LoadTranslationFilesProcess(
                        const std::set<std::filesystem::path>& _Path,
                        const std::function<void()>&           _OnFinished = nullptr,
                        const std::function<void()>&           _OnCanceled = nullptr, 
                        const std::function<void()>&           _OnFailed   = nullptr);
                    
                    virtual ~LoadTranslationFilesProcess();
                
                    // getters
                    std::map<
                        std::filesystem::path, 
                        std::map<std::string, std::string>>& get_translation_files()
                    {
                        std::unique_lock<std::mutex> lock(m_Mutex);
                        return m_TranslationFiles;
                    }

                    //
                    virtual bool awake() override;

                    // Async::IProcessStatus
                    virtual std::string iprocess_status_request_status() override;

                    // Async::IProcessProgress
                    virtual float iprocess_progress_request_progress() override;

                protected:
                    // info
                    std::map<
                        std::filesystem::path, 
                        std::map<std::string, std::string>> m_TranslationFiles;

                    float       m_Progress = 0.f;
                    std::string m_Status   = std::string();
                };

                // SaveTranslationFiles
                class SaveTranslationFilesProcess :
                    public Async::Process, 
                    public Async::IProcessStatus,
                    public Async::IProcessProgress
                {
                public:
                    SaveTranslationFilesProcess(
                        const std::map<std::filesystem::path, std::map<std::string, std::string>>& _Translations,
                        const std::function<void()>& _OnFinished  = nullptr,
                        const std::function<void()>& _OnCanceled  = nullptr, 
                        const std::function<void()>& _OnFailed    = nullptr);
                    
                    virtual ~SaveTranslationFilesProcess();
                
                    virtual bool awake() override;

                    // Async::IProcessStatus
                    virtual std::string iprocess_status_request_status() override;

                    // Async::IProcessProgress
                    virtual float iprocess_progress_request_progress() override;

                protected:

                    // info
                    std::map<
                        std::filesystem::path, 
                        std::map<std::string, std::string>> m_TranslationFiles;

                    float       m_Progress = 0.f;
                    std::string m_Status   = std::string();
                };
            }

            namespace Tools
            {
                class TranslationFilesEditor : public Layer
                {
                public:
                    TranslationFilesEditor();
                    virtual ~TranslationFilesEditor();

                    virtual void frame_update() override;
                    virtual void finish() override;
                
                protected:
                    std::filesystem::path m_TranslationFilesLocation;

                    Frenchie::Core::Reference<Async::LoadTranslationFilesProcess> m_LoadProcess;
                    Frenchie::Core::Reference<Async::SaveTranslationFilesProcess> m_SaveProcess;
                };
            }
        }
    }
}