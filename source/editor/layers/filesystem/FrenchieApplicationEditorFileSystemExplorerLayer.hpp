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
            // Explorer
            class FileSystemExplorer : 
                public Layer, 
                public Frenchie::Core::Serialization::ISerializer
            {
            public:
                FileSystemExplorer();
                virtual ~FileSystemExplorer();

                // static API
                static std::set<std::filesystem::path> get_selected_paths();
                static void create_folder();
                static void copy_paths();
                static void paste_paths();
                static void remove_paths();
                static void rename_paths();

                // Layer
                virtual void frame_update() override;
                virtual bool allows_multiple_instances() const;

                // Frenchie::Core::Serialization::ISerializer
                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

            protected:

                // info
                InputText                       m_CurrentDirectory;
                InputText                       m_CurrentFile;
                bool                            m_DrawCurrentDirectoryTextEdit = false;
                std::set<std::filesystem::path> m_SelectedPaths;
                std::set<std::filesystem::path> m_CopiedPaths;

                // servive methods
                void change_current_directory(const std::filesystem::path&);
                void draw_current_directory_path_editor();
                void draw_current_filename_editor();
                void draw_current_directory_paths_table();
                void draw_current_directory_popup_menu();
                void handle_current_directory_hot_keys();
                void draw_paths_tree(const std::filesystem::path&, int&);
                void drop_path_to(const std::filesystem::path&);
                void drag_selected_paths(const std::filesystem::path&);
            };
        }
    }
}