#pragma once

// Core
#include <FrenchieCoreISerializer.hpp>
#include <FrenchieCoreThreadPool.hpp>

// Application
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
        }
    }
}