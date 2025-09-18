#pragma once

#include <FrenchieEditorFileSystemExplorerLayer.hpp>

namespace Frenchie
{
    namespace Editor
    {
        namespace FileSystem
        {
            // class FilesystemPathsSearchProcess : public Frenchie::Application::Thread
            // {
            // public:
            //     FilesystemPathsSearchProcess(
            //         const std::filesystem::path&                             _Path,
            //         const std::function<bool(const std::filesystem::path&)>& _Predicate,
            //         const std::string&                                       _Name           = STRINGIFY(FilesystemPathsSearchProcess),
            //         size_t                                                   _MaxSearchDepth = 100);

            //     virtual ~FilesystemPathsSearchProcess();

            //     virtual void execute() override;

            //     mutable std::filesystem::path                 m_CurrentPath =  std::filesystem::current_path();
            //     mutable std::map<std::filesystem::path, bool> m_Paths       = std::map<std::filesystem::path, bool>();

            // protected:
            //     mutable std::filesystem::path                             m_Path           =  std::filesystem::current_path();
            //     mutable std::function<bool(const std::filesystem::path&)> m_Predicate      = nullptr;
            //     mutable size_t                                            m_MaxSearchDepth = 4;
            // };

            class PathScannerDialog : public Frenchie::Editor::FileSystem::Explorer
            {
            public:
                PathScannerDialog(
                    const std::function<bool(const std::filesystem::path&)>& _Predicate,
                    const std::string&                                       _Name = STRINGIFY(Frenchie::Application::Editor::ScannerView));

                virtual ~PathScannerDialog();
                
                // Frenchie::Application::Layer
                virtual void frame_update() override;
                virtual bool allows_multiple_instances() const override;

                // API
                void on_finished(const std::function<void(std::map<std::filesystem::path, bool>&)>& _Callback);
                void on_canceled(const std::function<void(std::map<std::filesystem::path, bool>&)>& _Callback);
                void on_failed(const std::function<void(std::map<std::filesystem::path, bool>&)>& _Callback);

            protected:
                std::function<bool(const std::filesystem::path&)>           m_Predicate;
                std::function<void(std::map<std::filesystem::path, bool>&)> m_OnFinished;
                std::function<void(std::map<std::filesystem::path, bool>&)> m_OnCanceled;
                std::function<void(std::map<std::filesystem::path, bool>&)> m_OnFailed;
            };
        }
    }
}