#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>
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
                virtual void frame_update() override;

            protected:
                std::filesystem::path m_Path = std::filesystem::current_path();
                PathSelector          m_Selector;

                // service methods
                void draw_paths_tree(const std::filesystem::path&);
            };
        }
    }
}