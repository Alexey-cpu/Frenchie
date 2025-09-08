#pragma once

// Application
#include <FrenchieApplicationEditorAbstractDialogLayer.hpp>
#include <FrenchieApplicationEditorAsyncProcessLayer.hpp>
#include <FrenchieApplicationEditorInputText.hpp>
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplication.hpp>

// Core
#include <FrenchieCoreISerializer.hpp>
#include <FrenchieCoreThreadPool.hpp>

// IMGUI
#include <imgui.h>

// STL
#include <map>
#include <future>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            namespace Async
            {
                class FilesystemPathsSearchProcess : public Process
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
                    mutable std::filesystem::path                                       m_Path           =  std::filesystem::current_path();
                    mutable std::function<bool(const std::filesystem::path&)>           m_Predicate      = nullptr;
                    mutable std::filesystem::path                                       m_CurrentPath    =  std::filesystem::current_path();
                    mutable size_t                                                      m_MaxSearchDepth = 4;
                    mutable std::map<std::filesystem::path, bool>                       m_Paths          = std::map<std::filesystem::path, bool>();
                };
            }
        }
    }
}

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            namespace FileSystem
            {
                // Explorer
                class Explorer : public Layer
                {
                public:
                    Explorer(
                        const std::string&           = STRINGIFY(Explorer),
                        const std::filesystem::path& = std::filesystem::current_path());
                    virtual ~Explorer();

                    // static API
                    std::filesystem::path get_path() const;
                    std::set<std::filesystem::path> get_selected_paths() const;
                    void create_folder();
                    void copy_paths();
                    void paste_paths();
                    void remove_paths();
                    void rename_paths();

                    // Frenchie::Application::Layer
                    virtual void frame_update() override;
                protected:

                    // info
                    std::filesystem::path           m_Path                         = std::filesystem::current_path();
                    InputText                       m_CurrentDirectory;
                    InputText                       m_CurrentFile;
                    bool                            m_DrawCurrentDirectoryTextEdit = false;
                    mutable std::set<std::filesystem::path> m_SelectedPaths;
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

                namespace Dialogs
                {
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

                        std::function<void()>                                m_Launcher = nullptr;
                        std::shared_ptr<Async::FilesystemPathsSearchProcess> m_Process  = nullptr;
                    };
                }
            }
        }
    }
}