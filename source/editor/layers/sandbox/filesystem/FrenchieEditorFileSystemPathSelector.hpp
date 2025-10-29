#pragma once

// Core
#include <FrenchieCoreStringUnicode.hpp>

// Application

#include <filesystem>
#include <set>

namespace Frenchie
{
    namespace Editor
    {
        namespace FileSystem
        {
            class PathSelector final
            {
            public:
                PathSelector();
                ~PathSelector();

                // getters
                std::set<std::filesystem::path> get_selected_paths() const;
                std::u32string get_current_file_path() const;
                

                // API
                bool contains(const std::filesystem::path&) const;
                void select(const std::filesystem::path&);
                void clear();

            protected:
                mutable std::u32string                  m_CurrentFile;
                mutable std::set<std::filesystem::path> m_SelectedPaths;
            };
        }
    }
}