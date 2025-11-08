#pragma once

// Application
#include <FrenchieApplication.hpp>

// Editor
#include <FrenchieEditorFileSystemPathSelector.hpp>

// STL
#include <filesystem>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

namespace Frenchie
{
    namespace Editor
    {
        namespace FileSystem
        {
            namespace Menu
            {
                namespace Selection{}

                namespace Folder{}

                namespace File{}
            }
        }
    }
}

namespace Frenchie
{
    namespace Editor
    {
        namespace FileSystem
        {
            class DirectoryTree : public Frenchie::Application::Layer
            {
            public:
                DirectoryTree(const std::filesystem::path& = "C:/SDK/Qt_Projects/OpenGL/logs");
                virtual ~DirectoryTree();

                // Frenchie::Application::Layer
                virtual bool awake() override;
                virtual void frame_update() override;

            protected:
                std::filesystem::path m_RootPath    = std::filesystem::current_path();
                std::filesystem::path m_FocusedPath = std::filesystem::current_path();
                PathSelector          m_Selector    = PathSelector();

                // service methods
                void draw_paths_tree(const std::filesystem::path&);

                void drop_paths_to(const std::filesystem::path& _Path);

                void drag_paths();

                std::shared_ptr<Frenchie::Application::PlatformBackendRendererTexture> default_file;
                std::shared_ptr<Frenchie::Application::PlatformBackendRendererTexture> default_folder;
                std::shared_ptr<Frenchie::Application::PlatformBackendRendererTexture> default_folder_opened;
            };
        }
    }
}