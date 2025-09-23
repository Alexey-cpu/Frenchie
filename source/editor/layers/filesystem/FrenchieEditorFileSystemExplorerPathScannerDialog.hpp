#pragma once

#include <FrenchieEditorFileSystemExplorer.hpp>

namespace Frenchie
{
    namespace Editor
    {
        namespace FileSystem
        {
            class PathScannerDialog : public Frenchie::Editor::FileSystem::Explorer
            {
            public:
                PathScannerDialog(
                    const std::function<bool(const std::filesystem::path&)>& _Predicate,
                    const std::string&                                       _Name     = STRINGIFY(Frenchie::Application::Editor::ScannerView));

                virtual ~PathScannerDialog();
                
                // Frenchie::Application::Layer
                virtual bool awake() override;
                virtual void frame_update() override;
                virtual void finish();
                virtual bool allows_multiple_instances() const override;

                // API
                void on_accepted(const std::function<void(const std::map<std::filesystem::path, bool>&)>& _Callback);

            protected:
                std::function<bool(const std::filesystem::path&)>                 m_Predicate;
                int                                                               m_MaximumSearchDepth = 4;
                std::function<void(const std::map<std::filesystem::path, bool>&)> m_OnAccepted;
                Frenchie::Core::Reference<Frenchie::Application::ThreadQueue>     m_ThreadQueue;
                Frenchie::Core::Reference<Frenchie::Application::Thread>          m_Thread;
                std::map<std::filesystem::path, bool>                             m_Paths;
                std::mutex                                                        m_Mutex;
                std::filesystem::path                                             m_CurrentlyProcessedPath;
            };
        }
    }
}