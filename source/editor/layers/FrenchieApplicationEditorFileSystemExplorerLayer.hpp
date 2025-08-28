#pragma once

// Application
#include <FrenchieApplicationEditorInputTextDrawer.hpp>
#include <FrenchieApplicationCommandsQueueLayer.hpp>
#include <FrenchieApplicationEditorMenuDrawer.hpp>
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
                // FileMenu
                class FileMenu : public Layer
                {
                public:
                    FileMenu();
                    virtual ~FileMenu();

                    // Layer
                    virtual void frame_update() override;

                protected:
                    MenuDrawer m_MenuDrawer;
                };

                // FolderMenu
                class FolderMenu : public Layer
                {
                public:
                    FolderMenu();
                    virtual ~FolderMenu();

                    // Layer
                    virtual void frame_update() override;

                protected:
                    MenuDrawer m_MenuDrawer;  
                };

                // Explorer
                class Explorer : public Layer
                {
                public:
                    Explorer();
                    virtual ~Explorer();

                    // API
                    static std::vector<std::filesystem::path> get_selected_paths();

                    // Layer
                    virtual void frame_update() override;
                    virtual bool allows_multiple_instances() const;

                protected:

                    // info
                    std::map<std::filesystem::path, bool> m_Paths = 
                        std::map<std::filesystem::path, bool>();

                    std::map<std::string, bool> m_FormatFilter = 
                        std::map<std::string, bool>();

                    InputText m_CurrentDirectoryTextEdit;
                    bool      m_DrawCurrentDirectoryTextEdit = false;

                    // servive methods
                    void change_current_directory(const std::filesystem::path&);
                    void draw_current_directory_path_editor();
                    void draw_current_directory_paths_table();
                    void draw_current_directory_format_filter();
                    void draw_current_directory_popup_menu();
                    void handle_current_directory_hot_keys();
                    void draw_paths_tree(const std::filesystem::path&, int&);

                    void drop_item_to(const std::filesystem::path&);
                };   
            }
        }
    }
}