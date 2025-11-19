#include <FrenchieEditorFileSystemPathSelector.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

PathSelector::PathSelector(){}
PathSelector::~PathSelector(){}

std::set<std::filesystem::path> PathSelector::get_selected_paths() const
{
    auto selection = m_SelectedPaths;

    for(auto&& path : selection)
    {
        if(!std::filesystem::exists(path))
            m_SelectedPaths.erase(path);
    }

    return m_SelectedPaths;
}

std::u32string PathSelector::get_current_file_path() const
{
    return m_CurrentFile;
}

bool PathSelector::contains(const std::filesystem::path& _Path) const
{
    return m_SelectedPaths.find(_Path) != m_SelectedPaths.end();
}

void PathSelector::select(const std::filesystem::path& _Path)
{
    m_SelectedPaths.insert(_Path);
    m_CurrentFile = _Path.filename().u32string();
}

bool PathSelector::empty() const
{
    return get_selected_paths().empty();
}

void PathSelector::clear()
{
    m_SelectedPaths.clear();
    m_CurrentFile = U"...";
}