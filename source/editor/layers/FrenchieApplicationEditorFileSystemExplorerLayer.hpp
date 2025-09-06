#pragma once

// Application
#include <FrenchieApplicationEditorInputText.hpp>
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplication.hpp>

// Core
#include <FrenchieCoreISerializer.hpp>

// IMGUI
#include <imgui.h>

// STL
#include <map>

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
                    class GetOpenFiles : public FileSystem::Explorer 
                    {
                    public:
                        GetOpenFiles(
                            const std::function<void()>& _OnAccpected, 
                            const std::string&           _Name = STRINGIFY(GetOpenFiles));
                        
                        virtual ~GetOpenFiles();

                        // Frenchie::Application::Layer
                        virtual void frame_update() override;
                        virtual bool allows_multiple_instances() const override;

                    protected:
                        std::function<void()> m_OnAccepted = nullptr;
                    };
                }
            }
        }
    }
}