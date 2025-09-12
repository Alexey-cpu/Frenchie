#pragma once

// Core
#include <FrenchieCoreISerializer.hpp>
#include <FrenchieCoreThreadPool.hpp>

// Application
#include <FrenchieApplicationAsynchronousProcessesLayer.hpp>
#include <FrenchieApplicationEditorAbstractDialogLayer.hpp>
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplication.hpp>

// Editor
#include <FrenchieEditorConfigurationTranslatorLayer.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>

// STL
#include <map>
#include <future>

namespace Frenchie
{
    namespace Editor
    {
        namespace FileSystem
        {
            using namespace Configuration;

            // Explorer
            class Explorer : public Frenchie::Application::Layer
            {
            public:
                Explorer(
                    const std::string&           = STRINGIFY(Explorer),
                    const std::filesystem::path& = std::filesystem::current_path());
                virtual ~Explorer();

                // static API
                std::set<std::filesystem::path> get_selected_paths() const;
                std::filesystem::path get_current_path() const;
                std::filesystem::path get_current_file() const;
                void create_folder();
                void copy_paths();
                void paste_paths();
                void remove_paths();
                void rename_paths();

                // Frenchie::Application::Layer
                virtual void frame_update() override;

            protected:

                class Selection
                {
                public:

                    std::set<std::filesystem::path> get_selected_paths() const
                    {
                        auto selection = m_SelectedPaths;

                        for(auto&& path : selection)
                        {
                            if(!std::filesystem::exists(path))
                                m_SelectedPaths.erase(path);
                        }

                        return m_SelectedPaths;
                    }

                    void select_path(const std::filesystem::path& _Path)
                    {
                        m_SelectedPaths.insert(_Path);
                        m_CurrentFile = Frenchie::Core::String::as_utf8(_Path.filename().wstring());
                    }

                    void clear_selection()
                    {
                        m_SelectedPaths.clear();
                        m_CurrentFile = Translator::translate("Nothing selected...");
                    }

                    bool contains(const std::filesystem::path& _Path) const
                    {
                        return m_SelectedPaths.find(_Path) != m_SelectedPaths.end();
                    }

                    mutable std::string m_CurrentFile;

                protected:
                    mutable std::set<std::filesystem::path> m_SelectedPaths;
                } m_Selection;

                // info
                std::filesystem::path                   m_Path = std::filesystem::current_path();
                std::string                             m_CurrentDirectory;
                bool                                    m_DrawCurrentDirectoryTextEdit = false;
                mutable std::set<std::filesystem::path> m_CopiedPaths;

                // servive methods
                void change_current_directory(const std::filesystem::path&);
                
                // drawers
                void draw_contents();
                void draw_current_directory_path_editor();
                void draw_current_filename_editor();
                void draw_current_directory_paths_table();
                void draw_current_directory_popup_menu();
                void handle_current_directory_hot_keys();
                void draw_paths_tree(const std::filesystem::path&);
                void drop_path_to(const std::filesystem::path&);
                void drag_selected_paths(const std::filesystem::path&);
            };

            class ExplorerDialog : public FileSystem::Explorer 
            {
            public:
                ExplorerDialog(const std::string& _Name = STRINGIFY(ExplorerDialog));
                virtual ~ExplorerDialog();

                // Frenchie::Application::Layer
                virtual void frame_update() override;
                virtual bool allows_multiple_instances() const override;

                // API
                void on_accepted(const std::function<void()>& _Callback)
                {
                    m_OnAccepted = _Callback;
                }
                
                void on_canceled(const std::function<void()>& _Callback)
                {
                    m_OnCanceled = _Callback;
                }

            protected:
                std::function<void()> m_OnAccepted = nullptr;
                std::function<void()> m_OnCanceled = nullptr;
            };

            class FilesystemPathsSearchProcess : public Frenchie::Application::Process
            {
            public:
                FilesystemPathsSearchProcess(
                    const std::filesystem::path&                             _Path,
                    const std::function<bool(const std::filesystem::path&)>& _Predicate,
                    const std::string&                                       _Name           = STRINGIFY(FilesystemPathsSearchProcess),
                    size_t                                                   _MaxSearchDepth = 100);

                virtual ~FilesystemPathsSearchProcess();

                virtual bool awake() override;

                mutable std::filesystem::path                 m_CurrentPath =  std::filesystem::current_path();
                mutable std::map<std::filesystem::path, bool> m_Paths       = std::map<std::filesystem::path, bool>();

            protected:
                mutable std::filesystem::path                             m_Path           =  std::filesystem::current_path();
                mutable std::function<bool(const std::filesystem::path&)> m_Predicate      = nullptr;
                mutable size_t                                            m_MaxSearchDepth = 4;
            };

            class PathScannerDialog : public Explorer
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
                std::function<void(std::map<std::filesystem::path, bool>&)> m_OnFinished;
                std::function<void(std::map<std::filesystem::path, bool>&)> m_OnCanceled;
                std::function<void(std::map<std::filesystem::path, bool>&)> m_OnFailed;
            };
        }
    }
}