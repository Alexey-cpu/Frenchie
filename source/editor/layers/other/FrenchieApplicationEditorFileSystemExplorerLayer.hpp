#pragma once

// Application
#include <FrenchieApplicationEditorAbstractDialogLayer.hpp>
#include <FrenchieApplicationProcessesLayer.hpp>
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplication.hpp>

// Core
#include <FrenchieCoreISerializer.hpp>
#include <FrenchieCoreThreadPool.hpp>

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
            class FilesystemPathsSearchProcess : public Frenchie::Application::Process
            {
            public:
                FilesystemPathsSearchProcess(
                    const std::filesystem::path&                                       _Path,
                    const std::function<bool(const std::filesystem::path&)>&           _Predicate,
                    const std::function<void(std::map<std::filesystem::path, bool>&)>& _OnFinished     = nullptr,
                    const std::function<void(std::map<std::filesystem::path, bool>&)>& _OnCanceled     = nullptr,
                    const std::function<void(std::map<std::filesystem::path, bool>&)>& _OnFailed       = nullptr,
                    const std::string&                                                 _Name           = STRINGIFY(FilesystemPathsSearchProcess),
                    size_t                                                             _MaxSearchDepth = 100);

                virtual ~FilesystemPathsSearchProcess();

                std::filesystem::path get_current_path() const;
                std::map<std::filesystem::path, bool> get_paths() const;

                virtual bool awake() override;

            protected:
                mutable std::filesystem::path                             m_Path           =  std::filesystem::current_path();
                mutable std::function<bool(const std::filesystem::path&)> m_Predicate      = nullptr;
                mutable std::filesystem::path                             m_CurrentPath    =  std::filesystem::current_path();
                mutable size_t                                            m_MaxSearchDepth = 4;
                mutable std::map<std::filesystem::path, bool>             m_Paths          = std::map<std::filesystem::path, bool>();
            };

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
                        m_CurrentFile = "Nothing selected...";
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
                ExplorerDialog(
                    const std::string&           _Name        = STRINGIFY(ExplorerDialog),
                    const std::function<void()>& _OnAccpected = nullptr,
                    const std::function<void()>& _OnCanceled  = nullptr);
                
                virtual ~ExplorerDialog();

                // Frenchie::Application::Layer
                virtual void frame_update() override;
                virtual bool allows_multiple_instances() const override;

            protected:
                std::function<void()> m_OnAccepted = nullptr;
                std::function<void()> m_OnCanceled = nullptr;
            };

            class PathScannerDialog : public Dialog
            {
            public:
                PathScannerDialog(
                    const std::filesystem::path&                                       _Path,
                    const std::function<bool(const std::filesystem::path&)>&           _Predicate,
                    const std::function<void(std::map<std::filesystem::path, bool>&)>& _OnFinished     = nullptr,
                    const std::function<void(std::map<std::filesystem::path, bool>&)>& _OnCanceled     = nullptr,
                    const std::function<void(std::map<std::filesystem::path, bool>&)>& _OnFailed       = nullptr,
                    const std::string&                                                 _Name           = STRINGIFY(Frenchie::Application::Editor::ScannerView),
                    size_t                                                             _MaxSearchDepth = 100);
                virtual ~PathScannerDialog();
                
                virtual bool awake() override;
                virtual void frame_update() override;
                virtual void draw_content() override;
                virtual void draw_buttons() override;
                virtual void finish() override;

            protected:

                std::function<void()>                         m_Launcher = nullptr;
                std::shared_ptr<FilesystemPathsSearchProcess> m_Process  = nullptr;
            };
        }
    }
}