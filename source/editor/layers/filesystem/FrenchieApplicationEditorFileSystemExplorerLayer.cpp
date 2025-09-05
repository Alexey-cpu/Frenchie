#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

#include <FrenchieApplication.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerFileMenu.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerFolderMenu.hpp>
#include <FrenchieApplicationEditorFileSystemFilesRenameDialogLayer.hpp>
#include <FrenchieApplicationEditorFileSystemFilesRemoveDialogLayer.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

// Explorer
FileSystemExplorer::FileSystemExplorer(
    const std::string&           _Name,
    const std::filesystem::path& _Path) : 
    Layer(_Name), 
    m_Path((std::filesystem::exists(_Path) ? _Path : std::filesystem::current_path())){}

FileSystemExplorer::~FileSystemExplorer(){}

std::filesystem::path FileSystemExplorer::get_path() const
{
    return m_Path;
}

std::set<std::filesystem::path> FileSystemExplorer::get_selected_paths() const
{    
    auto selection = m_SelectedPaths;

    for(auto&& path : selection)
    {
        if(!std::filesystem::exists(path))
            m_SelectedPaths.erase(path);
    }

    return m_SelectedPaths;
}

void FileSystemExplorer::create_folder()
{
    std::wstring newFolderName = L"NewFolder";

    auto newPath = m_Path.wstring().append(L"/").append(newFolderName);

    while(std::filesystem::exists(newPath))
        newPath = newPath.append(L"_Copy");

    try
    {
        std::filesystem::create_directories(newPath);
    }
    catch(const std::exception& e)
    {
        Frenchie::Core::Logger::instance()->critical(e.what());
    }
}

void FileSystemExplorer::copy_paths()
{
    m_CopiedPaths = get_selected_paths();
}

void FileSystemExplorer::paste_paths()
{
    for(auto&& path : m_CopiedPaths)
    {
        if(path.empty())
            continue;

        if(!std::filesystem::exists(path)) 
        {
            Frenchie::Core::Logger::instance()->critical(fmt::format("{} does not exists", path.string()));
            continue;
        }

        auto source    = std::filesystem::path(path);
        auto extention = Frenchie::Core::FileSystem::get_file_extention(source);
        auto target    = std::filesystem::path(
            m_Path.wstring()
            .append(L"/")
            .append(source.filename().stem().wstring())
            .append(L"_Copy")
            .append(Frenchie::Core::String::as_wide(extention))).make_preferred();

        while(std::filesystem::exists(target))
        {
            target = std::filesystem::path(
                m_Path.wstring()
                .append(L"/")
                .append(target.filename().stem().wstring())
                .append(L"_Copy")
                .append(Frenchie::Core::String::as_wide(extention))).make_preferred();
        }

        // try to copy
        try
        {
            std::filesystem::copy(source, target);
        }
        catch(const std::exception& e)
        {
            Frenchie::Core::Logger::instance()->critical(e.what());
        }
    }
}

void FileSystemExplorer::remove_paths()
{
    auto selectedPaths = get_selected_paths();

    if(!selectedPaths.empty()) 
        Application::instance()->push<FilesRemoveDialog>(selectedPaths);
}

void FileSystemExplorer::rename_paths()
{
    auto selectedPaths = get_selected_paths();

    if(!selectedPaths.empty()) 
        Application::instance()->push<FilesRenameDialog>(selectedPaths);
}

void FileSystemExplorer::frame_update()
{
    // draw content
    ImGui::Begin(get_name().c_str(), &m_Shown);
    {
        draw_contents();
        ImGui::End();
    }
}

// Frenchie::Core::Serialization::ISerializer
bool FileSystemExplorer::serialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    // write self
    auto self = _Parent.append_node(STRINGIFY(FileSystemExplorer));

    // write message content filter
    self.append_node(
        STRINGIFY(m_CurrentDirectory), 
        Frenchie::Core::String::as_utf8(m_Path.wstring()).c_str(),
        Frenchie::Core::Serialization::NodeType::OBJECT);

    return true;
}

bool FileSystemExplorer::deserialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto self = _Parent.find_node(STRINGIFY(FileSystemExplorer));

    if(self.empty()) 
        return false;

    // try to setup current directory
    try
    {
        std::filesystem::current_path(std::filesystem::path(
                self.find_node(STRINGIFY(m_CurrentDirectory)).get_value()
            )
        );
    }
    catch(const std::exception& e)
    {
        Frenchie::Core::Logger::instance()->critical(e.what());
    }

    return true;
}

void FileSystemExplorer::change_current_directory(const std::filesystem::path& _Path)
{
    if(!std::filesystem::exists(_Path)) 
    {
        Frenchie::Core::Logger::instance()->critical(fmt::format("{} does not exist", _Path.string()));
        return;
    }

    if(!std::filesystem::is_directory(_Path))
    {
        Frenchie::Core::Logger::instance()->critical(fmt::format("{} is not a directory", _Path.string()));
        return;
    }

    Application::instance()
        ->find_or_push<Frenchie::Application::CommandsQueue>()
        ->push<Frenchie::Application::CallbackCommand>(
            [this, _Path]()
            {
                try
                {
                    // change current path
                    auto path = _Path;

                    m_Path = path.make_preferred();

                    // clear selection
                    m_SelectedPaths.clear();
                }
                catch(const std::exception& e)
                {
                    Frenchie::Core::Logger::instance()->critical(e.what());
                }
            }
        );
}

void FileSystemExplorer::draw_contents()
{
    // check that current path exists
    while(!std::filesystem::exists(m_Path))
        m_Path = m_Path.parent_path();

    if(ImGui::BeginTable("FileSystemContentTable", 2,
        ImGuiTableFlags_::ImGuiTableFlags_ScrollY      | 
        ImGuiTableFlags_::ImGuiTableFlags_RowBg        | 
        ImGuiTableFlags_::ImGuiTableFlags_BordersOuter | 
        ImGuiTableFlags_::ImGuiTableFlags_BordersV     |
        ImGuiTableFlags_::ImGuiTableFlags_Resizable    |
        ImGuiTableFlags_::ImGuiTableFlags_Reorderable  |
        ImGuiTableFlags_::ImGuiTableFlags_Hideable))
    {
        ImGui::TableNextRow();

        // draw tree
        ImGui::TableSetColumnIndex(0);

        ImGui::BeginChild("ContentTree");
        {
            draw_paths_tree(m_Path.root_path());
        }
        ImGui::EndChild();

        // draw table
        ImGui::TableSetColumnIndex(1);

        ImGui::BeginChild("ContentTable");
        {
            draw_current_directory_path_editor();
            draw_current_directory_paths_table();
            draw_current_directory_popup_menu();
            handle_current_directory_hot_keys();
            draw_current_filename_editor();
        }
        ImGui::EndChild();

        ImGui::EndTable();
    }
}

void FileSystemExplorer::draw_current_directory_path_editor()
{
    float height   = ImGui::CalcTextSize("LAGEST").y + 2.f * ImGui::GetStyle().FramePadding.y;
    bool  selected = false;

    // draw selectable
    ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyle().Colors[ImGuiCol_Header]);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyle().Colors[ImGuiCol_Header]);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImGui::GetStyle().Colors[ImGuiCol_Header]);

    if(ImGui::Selectable(
        "##", 
        &selected,
        ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowOverlap      | 
        ImGuiSelectableFlags_::ImGuiSelectableFlags_NoAutoClosePopups |
        ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowDoubleClick  | 
        ImGuiSelectableFlags_::ImGuiSelectableFlags_Highlight, 
        ImVec2(0.f, height)))
    {
        m_DrawCurrentDirectoryTextEdit = true;
    }

    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    if(!m_DrawCurrentDirectoryTextEdit)
    {
        // draw buttons
        auto path = m_Path;

        std::stack<std::filesystem::path> stack;

        while(true)
        {
            stack.push(path);

            if(path == path.parent_path()) 
                break;

            path = path.parent_path();
        }

        int buttonID = 0;

        while(!stack.empty())
        {
            ImGui::SameLine();

            auto path = stack.top();
            auto name = Frenchie::Core::String::as_utf8(path.filename().wstring());

            if(name.empty()) 
                name = Frenchie::Core::String::as_utf8(path.wstring());

            ImGui::PushID(buttonID++);

            if(ImGui::Button(name.c_str(), ImVec2(0.f, height)))
                change_current_directory(path);

            ImGui::PopID();

            stack.pop();
        }

        // clear directory text edit
        m_CurrentDirectory.clear();
    }
    else
    {
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        // draw current path editor
        if(m_CurrentDirectory.empty())
            m_CurrentDirectory.set_buffer(Frenchie::Core::String::as_utf8(m_Path.make_preferred().wstring()));

        if(m_CurrentDirectory.draw("###", ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
        {            
            change_current_directory(
                std::filesystem::path(Frenchie::Core::String::as_wide(m_CurrentDirectory.get_buffer())));

            m_DrawCurrentDirectoryTextEdit = false;
        }

        if(!ImGui::IsItemHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_RectOnly) && 
            ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            m_DrawCurrentDirectoryTextEdit = false;
        }
    }
}

void FileSystemExplorer::draw_current_filename_editor()
{
    m_CurrentFile.set_buffer(
        m_SelectedPaths.empty() ? "No file selected..." : Frenchie::Core::String::as_utf8((*m_SelectedPaths.begin()).filename().wstring()));

    m_CurrentFile.draw("CurrentFile");
}

void FileSystemExplorer::draw_current_directory_paths_table()
{
    int pathsCount = 0;

    for(const auto& directory :
        std::filesystem::directory_iterator(m_Path.make_preferred(), 
        std::filesystem::directory_options::skip_permission_denied))
    {
        pathsCount++;
    }

    if (ImGui::BeginTable(
            "CurrentDirectoryContentTable",
            3,
            ImGuiTableFlags_::ImGuiTableFlags_ScrollY      | 
            ImGuiTableFlags_::ImGuiTableFlags_RowBg        | 
            ImGuiTableFlags_::ImGuiTableFlags_BordersOuter | 
            ImGuiTableFlags_::ImGuiTableFlags_BordersV     |
            ImGuiTableFlags_::ImGuiTableFlags_Resizable    |
            ImGuiTableFlags_::ImGuiTableFlags_Reorderable  |
            ImGuiTableFlags_::ImGuiTableFlags_Hideable, 
            ImVec2(0.0, ImGui::GetContentRegionAvail().y - 2.0f * ImGui::GetTextLineHeightWithSpacing())))
    {
        // setup columns
        ImGui::TableSetupColumn("name", 
            ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch |
            ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);
        
        ImGui::TableSetupColumn("last write time", 
            ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthFixed |
            ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);

        ImGui::TableSetupColumn("type", 
            ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
        
        ImGui::TableHeadersRow();

        // draw content of current directory
        auto pathIterator = 
            std::filesystem::directory_iterator(m_Path.make_preferred(), 
                std::filesystem::directory_options::skip_permission_denied);

        ImGuiListClipper clipper;
        clipper.Begin(pathsCount);

        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++, pathIterator++)
            {
                auto path = (*pathIterator).path();

                ImGui::TableNextRow();

                // draw name
                ImGui::TableSetColumnIndex(0);

                bool selected = m_SelectedPaths.find(path) != m_SelectedPaths.end();

                if(ImGui::Selectable(
                    Frenchie::Core::String::as_utf8(path.filename().wstring()).c_str(), 
                    &selected,
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns    | 
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowOverlap      | 
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_NoAutoClosePopups |
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowDoubleClick))
                {
                    if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) || 
                        ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightCtrl))
                    {
                        m_SelectedPaths.insert(path);
                    }
                    else
                    {
                        m_SelectedPaths.clear();
                        m_SelectedPaths.insert(path);
                    }

                    if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
                        change_current_directory(path);
                }

                // drag & drop
                drag_selected_paths(path);
                drop_path_to(path);

                // show pop up menu when item is clicked
                if(ImGui::IsItemClicked(ImGuiMouseButton_::ImGuiMouseButton_Right))
                {
                    // // select this item
                    m_SelectedPaths.insert(path);

                    // show context menu
                    draw_current_directory_popup_menu();
                }

                // draw last write time
                ImGui::TableSetColumnIndex(1);

                try
                {
                    auto time    = std::filesystem::last_write_time(path);
                    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
                    ImGui::TextUnformatted(asctime(std::localtime(&seconds)));
                }
                catch(const std::exception& e)
                {
                    Frenchie::Core::Logger::instance()->critical(e.what());
                    ImGui::TextUnformatted("UNKNOWN");
                }

                // draw type
                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted((std::filesystem::is_directory(path) ? "folder" : "file"));

            }
        }

        ImGui::EndTable();
    }
}

void FileSystemExplorer::draw_current_directory_popup_menu()
{
    if(ImGui::BeginPopupContextItem())
    {
        if(get_selected_paths().empty())
            FileSystemExplorerFolderMenu().draw(this);
        else 
            FileSystemExplorerFileMenu().draw(this);
        
        ImGui::EndPopup();
    }
}

void FileSystemExplorer::handle_current_directory_hot_keys()
{
    if(Application::instance()->find<Dialog>() != nullptr) 
        return;

    // Ctrl + C
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_C) && 
        (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightCtrl)))
    {            
        Frenchie::Application::Application::instance()->find_or_push<CommandsQueue>()->push(
            Frenchie::Application::Editor::FileMenuCopyAction::factory_id(), this);
    }

    // Ctrl + V
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_V) && 
        (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightCtrl)))
    {            
        Frenchie::Application::Application::instance()->find_or_push<CommandsQueue>()->push(
            Frenchie::Application::Editor::FileMenuPasteAction::factory_id(), this);
    }

    // Delete
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete))
    {            
        Frenchie::Application::Application::instance()->find_or_push<CommandsQueue>()->push(
            Frenchie::Application::Editor::FileMenuRemoveAction::factory_id(), this);
    }

    // Escape
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
    {
        m_SelectedPaths.clear();
    }
}

void FileSystemExplorer::draw_paths_tree(const std::filesystem::path& _Path)
{
    try
    {
        for(const auto& directory : 
            std::filesystem::directory_iterator(_Path));
    }
    catch(...)
    {
        return;
    }

    if(_Path == m_Path.root_path() || _Path == m_Path)
        ImGui::SetNextItemOpen(true);

    auto name = _Path.filename().wstring();

    drag_selected_paths(_Path);

    if(ImGui::TreeNodeEx(pugi::as_utf8(name.empty() ? L"Root" : name).c_str(), 
        ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_SpanAvailWidth | 
        ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DrawLinesFull  | 
        ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnDoubleClick))
    {
        // change current direcrtory on double click
        if(ImGui::IsItemHovered() && 
            ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            change_current_directory(_Path);
        }

        // drag & drop
        drop_path_to(_Path);

        // draw next item
        for(const auto& directory :
            std::filesystem::directory_iterator(_Path, std::filesystem::directory_options::skip_permission_denied))
        {
            if(directory.is_directory())
                draw_paths_tree(directory.path());
        }

        ImGui::TreePop();
    }
};

void FileSystemExplorer::drag_selected_paths(const std::filesystem::path& _Path)
{
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        std::set<std::filesystem::path> selection;
        auto selectedPaths = get_selected_paths();
        selectedPaths.insert(_Path);

        std::string selectionBuffer;

        for(auto&& selectedPath : selectedPaths)
        {
            if(selection.find(selectedPath) != selection.end()) 
                continue;

            selection.insert(selectedPath);

            selectionBuffer.append(Frenchie::Core::String::as_utf8(selectedPath.wstring())).append(";");
        }

        ImGui::SetDragDropPayload(STRINGIFY(std::filesystem::path),selectionBuffer.c_str(), selectionBuffer.size() + 1);
        ImGui::TextUnformatted(selectionBuffer.c_str());
        ImGui::EndDragDropSource();
    }
}

void FileSystemExplorer::drop_path_to(const std::filesystem::path& _Path)
{
    // drag & drop
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = 
            ImGui::AcceptDragDropPayload(STRINGIFY(std::filesystem::path)))
        {
            auto adress = static_cast<char*>(payload->Data);

            Frenchie::Core::Logger::instance()->info(fmt::format("moving\n{}", std::string(adress)));

            if(!std::filesystem::is_directory(_Path)) 
            {
                Frenchie::Core::Logger::instance()->critical(fmt::format("{} is not directory", _Path.string().c_str()));
                return;
            }
            
            auto movedPaths = Frenchie::Core::String::split(std::string(adress), ";");

            for(auto&& movedPath : movedPaths)
            {
                std::filesystem::path oldAdress(Frenchie::Core::String::as_wide(movedPath));

                if(!std::filesystem::exists(oldAdress)) 
                    continue;

                std::filesystem::path newAdress(_Path.wstring().append(L"/").append(oldAdress.filename()));

                if(oldAdress.parent_path() == newAdress.parent_path()) 
                    return;

                while(std::filesystem::exists(newAdress))
                {
                    newAdress = 
                        _Path.wstring()
                        .append(L"/")
                        .append(newAdress.filename().stem().wstring())
                        .append(L"_Copy")
                        .append(Frenchie::Core::String::as_wide(Frenchie::Core::FileSystem::get_file_extention(oldAdress)));
                }

                try
                {
                    std::filesystem::rename(oldAdress, newAdress);
                }
                catch(const std::exception& e)
                {
                    Frenchie::Core::Logger::instance()->critical(e.what());
                }
            }
        }

        ImGui::EndDragDropTarget();
    }
}