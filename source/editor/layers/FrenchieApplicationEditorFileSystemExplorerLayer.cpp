#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

#include <FrenchieApplication.hpp>
#include <FrenchieApplicationEditorDialogLayer.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;
using namespace Frenchie::Application::Editor::FileSystem;

// FileSystem

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            namespace FileSystem
            {
                // FileRenamerDialog
                class FilesRenameDialog : public Dialog
                {
                public:
                    FilesRenameDialog(const std::vector<std::filesystem::path>& _Paths): 
                        Dialog("Rename files")
                    {
                        for(auto&& path : _Paths)
                            m_Paths.insert({path, {std::make_shared<InputText>(), true}});
                    }
                    
                    virtual ~FilesRenameDialog(){}

                    virtual void draw_content() override
                    {
                        int checkboxID = 0;

                        for(auto&& path : m_Paths)
                        {
                            ImGui::PushID(checkboxID++);
                            ImGui::Checkbox("##", &path.second.second);
                            ImGui::PopID();
                            ImGui::SameLine();

                            ImGui::PushID(checkboxID++);
                            path.second.first->draw(
                                "###", 
                                Frenchie::Core::Helpers::String::as_utf8(path.first.filename().wstring()).c_str());

                            ImGui::PopID();
                        }
                    }

                    virtual void draw_buttons() override
                    {
                        if(ImGui::Button("Apply"))
                        {
                            // rename files
                            for(auto& item : m_Paths)
                            {
                                auto source   = item.first;
                                auto text     = item.second.first->get_buffer();
                                auto selected = item.second.second;

                                if(!std::filesystem::exists(source) || !selected) 
                                    continue;

                                auto target = std::filesystem::path(
                                    source.parent_path().wstring()
                                    .append(L"/")
                                    .append(Frenchie::Core::Helpers::String::as_wide(text))
                                );

                                if(source == target) 
                                    continue;

                                while (std::filesystem::exists(target))
                                {
                                    auto extention = Frenchie::Core::Helpers::String::as_wide(
                                        Frenchie::Core::Helpers::get_file_extention(target));

                                    target = source.parent_path().wstring()
                                        .append(L"/")
                                        .append(target.filename().stem().wstring())
                                        .append(L"_Copy")
                                        .append(extention);
                                }
                                
                                try
                                {
                                    std::filesystem::rename(source, target);
                                }
                                catch(const std::exception& e)
                                {
                                    Frenchie::Core::Logger::instance()->critical(e.what());
                                }
                                
                            }

                            // close dialog
                            close();
                        }
                        
                        ImGui::SameLine();
                        
                        if(ImGui::Button("Cancel"))
                            close();
                    }

                protected:
                    std::map<
                        std::filesystem::path, 
                        std::pair<std::shared_ptr<InputText>, bool>> m_Paths;
                };

                // FileRenamerDialog
                class FilesRemoveDialog : public Dialog
                {
                public:
                    FilesRemoveDialog(const std::vector<std::filesystem::path>& _Paths) : 
                        Dialog("Are you sure you want to delete these files ?"), 
                        m_Paths(_Paths){}
                    
                    virtual ~FilesRemoveDialog(){}

                    virtual void draw_content() override
                    {
                        for(auto&& path : m_Paths)
                            ImGui::TextUnformatted(path.string().c_str());
                    }

                    virtual void draw_buttons() override
                    {
                        if(ImGui::Button("Yes"))
                        {
                            if(m_Paths.empty())
                                return;

                            for(auto& path : m_Paths)
                            {
                                try
                                {
                                    std::filesystem::remove_all(path);
                                }
                                catch(...)
                                {
                                    Frenchie::Core::Logger::instance()->critical(fmt::format("Could not remove {}", path.string()));
                                }
                            }

                            close();
                        }
                        
                        ImGui::SameLine();
                        
                        if(ImGui::Button("No"))
                            close();
                    }

                protected:
                    std::vector<std::filesystem::path> m_Paths;
                };

                //
                class FileMenuActions
                {
                public:
                    static void copy()
                    {
                        auto selectedPaths = Explorer::get_selected_paths();

                        if(selectedPaths.empty())
                            return;

                        std::string clipBoardText;

                        for(auto& path : selectedPaths)
                        {
                            clipBoardText = clipBoardText.append(
                                fmt::format("{}\n", Frenchie::Core::Helpers::String::as_utf8(path.wstring()).c_str()));
                        }

                        ImGui::SetClipboardText(clipBoardText.c_str());
                    }

                    static void paste()
                    {
                        auto paths = Frenchie::Core::Helpers::String::split(std::string(ImGui::GetClipboardText()), "\n");

                        for(auto&& path : paths)
                        {
                            if(!std::filesystem::exists(path)) 
                                continue;

                            auto source    = std::filesystem::path(path);
                            auto extention = Frenchie::Core::Helpers::get_file_extention(source);
                            auto target    = std::filesystem::path(
                                std::filesystem::current_path().wstring()
                                .append(L"/")
                                .append(source.filename().stem().wstring())
                                .append(L"_Copy")
                                .append(pugi::as_wide(extention))).make_preferred();

                            while(std::filesystem::exists(target))
                            {
                                target = std::filesystem::path(
                                    std::filesystem::current_path().wstring()
                                    .append(L"/")
                                    .append(target.filename().stem().wstring())
                                    .append(L"_Copy")
                                    .append(pugi::as_wide(extention))).make_preferred();
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

                    static void remove()
                    {
                        auto selectedPaths = Explorer::get_selected_paths();

                        if(!selectedPaths.empty()) 
                            Application::instance()->push<FilesRemoveDialog>(selectedPaths);
                    }

                    static void rename()
                    {
                        auto selectedPaths = Explorer::get_selected_paths();

                        if(!selectedPaths.empty()) 
                            Application::instance()->push<FilesRenameDialog>(selectedPaths);
                    }

                    static void create_folder()
                    {
                        std::wstring newFolderName = L"NewFolder";

                        auto newPath = std::filesystem::current_path().wstring().append(L"/").append(newFolderName);

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
                };

                class FileMenuCopyAction : 
                    public Frenchie::Core::Command::Registry<FileMenuCopyAction>
                {
                public:

                    // Frenchie::Core::Command
                    virtual void execute() override
                    {
                        FileMenuActions::copy();
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "copy");
                    }
                };

                class FileMenuPasteAction : 
                    public Frenchie::Core::Command::Registry<FileMenuPasteAction>
                {
                public:

                    virtual void execute() override
                    {
                        FileMenuActions::paste();
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "paste");
                    }
                };

                class FileMenuRemoveAction : 
                    public Frenchie::Core::Command::Registry<FileMenuRemoveAction>
                {
                public:

                    // Frenchie::Core::Command
                    virtual void execute() override
                    {
                        FileMenuActions::remove();
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "remove");
                    }
                };

                class FileMenuRenameAction : 
                    public Frenchie::Core::Command::Registry<FileMenuRenameAction>
                {
                public:

                    // Frenchie::Core::Command
                    virtual void execute() override
                    {
                        FileMenuActions::rename();
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "rename");
                    }
                };

                class FileMenuCreateFolderAction : 
                    public Frenchie::Core::Command::Registry<FileMenuCreateFolderAction>
                {
                public:

                    // Frenchie::Core::Command
                    virtual void execute() override
                    {
                        FileMenuActions::create_folder();
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "create::folder");
                    }
                };

                // Folder menu actions
                class FolderMenuCreateFolderAction : 
                    public Frenchie::Core::Command::Registry<FolderMenuCreateFolderAction>
                {
                public:

                    // Frenchie::Core::Command
                    virtual void execute() override
                    {
                        FileMenuActions::create_folder();
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FolderMenu), "create::folder");
                    }
                };

                class FolderMenuPasteAction : 
                    public Frenchie::Core::Command::Registry<FolderMenuPasteAction>
                {
                public:

                    // Frenchie::Core::Command
                    virtual void execute() override
                    {
                        FileMenuActions::paste();
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FolderMenu), "paste");
                    }
                };
            }
        }
    }
}

// FileMenu
FileMenu::FileMenu(){}
FileMenu::~FileMenu(){}

void FileMenu::frame_update()
{
    m_MenuDrawer.draw(STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu));
}

// FolderMenu
FolderMenu::FolderMenu(){}
FolderMenu::~FolderMenu(){}

void FolderMenu::frame_update()
{
    m_MenuDrawer.draw(STRINGIFY(Frenchie::Application::Editor::FileSystem::FolderMenu));
}

// Explorer
Explorer::Explorer() : 
    Layer(STRINGIFY(Explorer)){}

Explorer::~Explorer(){}

std::vector<std::filesystem::path> Explorer::get_selected_paths()
{
    auto explorer = Application::instance()->find<Explorer>();

    if(explorer == nullptr) 
        return std::vector<std::filesystem::path>();

    std::vector<std::filesystem::path> selectedPaths;
    
    for(auto&& path : explorer->m_Paths)
    {
        if(path.second) 
            selectedPaths.push_back(path.first);
    }

    return selectedPaths;
}

void Explorer::frame_update()
{
    // update paths and format filter
    std::map<std::filesystem::path, bool> paths;
    std::map<std::string, bool> formatFilter;

    for(const auto& directory :
        std::filesystem::directory_iterator(std::filesystem::current_path().make_preferred(), 
        std::filesystem::directory_options::skip_permission_denied))
    {
        // update path
        auto path   = directory.path();
        paths[path] = m_Paths.find(path) != m_Paths.end() ? m_Paths[path] : false;

        // update extention
        auto extention = Frenchie::Core::Helpers::get_file_extention(path);

        if(!extention.empty()) 
        {
            formatFilter[extention] = 
                m_FormatFilter.find(extention) != m_FormatFilter.end() ? m_FormatFilter[extention] : true;
        }
    }

    m_Paths        = paths;
    m_FormatFilter = formatFilter;

    // draw content
    ImGui::Begin(get_name().c_str(), &m_Shown);
    {
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

            ImGui::BeginChild("Tree");
            {
                int id = 0;
                draw_paths_tree(std::filesystem::current_path().root_path(), id);
            }
            ImGui::EndChild();

            // draw table
            ImGui::TableSetColumnIndex(1);

            ImGui::BeginChild("Table");
            {
                draw_current_directory_path_editor();
                draw_current_directory_paths_table();
                draw_current_directory_popup_menu();
                draw_current_directory_format_filter();
                handle_current_directory_hot_keys();
            }
            ImGui::EndChild();

            ImGui::EndTable();
        }

        ImGui::End();
    }
}

bool Explorer::allows_multiple_instances() const
{
    return false;
}

void Explorer::change_current_directory(const std::filesystem::path& _Path)
{
    // change current path
    if(!std::filesystem::is_directory(_Path) || !std::filesystem::exists(_Path)) 
        return;

    Application::instance()
        ->find_or_push<Frenchie::Application::CommandsQueue>()
        ->push<Frenchie::Core::CallbackCommand>(
            [this, _Path]()
            {
                try
                {
                    // change current path
                    auto path = _Path;

                    std::filesystem::current_path(path.make_preferred());
                }
                catch(const std::exception& e)
                {
                    Frenchie::Core::Logger::instance()->critical(e.what());
                }
            }
        );
}

void Explorer::draw_current_directory_path_editor()
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
        //ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns    | 
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
        auto path = std::filesystem::current_path();

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
            auto name = Frenchie::Core::Helpers::String::as_utf8(path.filename().wstring());

            if(name.empty()) 
                name = Frenchie::Core::Helpers::String::as_utf8(path.wstring());

            ImGui::PushID(buttonID++);

            if(ImGui::Button(name.c_str(), ImVec2(0.f, height)))
                change_current_directory(path);

            ImGui::PopID();

            stack.pop();
        }
    }
    else
    {
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        // draw current path editor
        if(m_CurrentDirectoryTextEdit.draw(
            "###",
            Frenchie::Core::Helpers::String::as_utf8(std::filesystem::current_path().make_preferred().wstring()).c_str(), 
            ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
        {
            change_current_directory(
                std::filesystem::path(Frenchie::Core::Helpers::String::as_wide(m_CurrentDirectoryTextEdit.get_buffer())));

            m_DrawCurrentDirectoryTextEdit = false;
        }   
    }
}

void Explorer::draw_current_directory_paths_table()
{
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
        auto widgetID = 0;

        for(auto&& path : m_Paths)
        {
            // check format filter
            if(!std::filesystem::is_directory(path.first))
            {
                auto iterator = m_FormatFilter.find(Frenchie::Core::Helpers::get_file_extention(path.first));

                if(iterator == m_FormatFilter.end() || !iterator->second) 
                    continue;
            }

            ImGui::TableNextRow();

            // draw name
            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(widgetID++);
            ImGui::Checkbox("##", &path.second);
            ImGui::PopID();
            ImGui::SameLine();

            ImGui::PushID(widgetID++);
            bool selected = false;

            if(ImGui::Selectable(
                Frenchie::Core::Helpers::String::as_utf8(path.first.filename().wstring()).c_str(), 
                &selected,
                ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns    | 
                ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowOverlap      | 
                ImGuiSelectableFlags_::ImGuiSelectableFlags_NoAutoClosePopups |
                ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowDoubleClick))
            {
                if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightCtrl))
                    path.second = !path.second;

                if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
                    change_current_directory(path.first);
            }

            //------------------------------------------------------------------------------------------------------------------
            // drag & drop
            //------------------------------------------------------------------------------------------------------------------
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                std::set<std::filesystem::path> selection;
                auto selectedPaths = get_selected_paths();
                selectedPaths.push_back(path.first);

                std::string selectionBuffer;

                for(auto&& selectedPath : selectedPaths)
                {
                    if(selection.find(selectedPath) != selection.end()) 
                        continue;

                    selection.insert(selectedPath);

                    selectionBuffer.append(Frenchie::Core::Helpers::String::as_utf8(selectedPath.wstring())).append("\n");
                }

                ImGui::SetDragDropPayload(STRINGIFY(std::filesystem::path),selectionBuffer.c_str(), selectionBuffer.size() + 1);
                ImGui::TextUnformatted(selectionBuffer.c_str());
                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = 
                    ImGui::AcceptDragDropPayload(STRINGIFY(std::filesystem::path)))
                {
                    auto adress = static_cast<char*>(payload->Data);

                    Frenchie::Core::Logger::instance()->info(fmt::format("moving\n{}", std::string(adress)));

                    if(std::filesystem::is_directory(path.first))
                    {
                        auto movedPaths = Frenchie::Core::Helpers::String::split(std::string(adress), "\n");

                        for(auto&& movedPath : movedPaths)
                        {
                            std::filesystem::path oldAdress(movedPath);

                            if(!std::filesystem::exists(oldAdress)) 
                                continue;

                            std::filesystem::path newAdress(path.first.wstring().append(L"/").append(oldAdress.filename()));

                            while(std::filesystem::exists(newAdress))
                            {
                                newAdress = 
                                    path.first.wstring()
                                    .append(L"/")
                                    .append(oldAdress.filename().stem().wstring())
                                    .append(L"_Copy")
                                    .append(Frenchie::Core::Helpers::String::as_wide(Frenchie::Core::Helpers::get_file_extention(oldAdress)));
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
                }

                ImGui::EndDragDropTarget();
            }
            //------------------------------------------------------------------------------------------------------------------

            // show pop up menu when item is clicked
            if(ImGui::IsItemClicked(ImGuiMouseButton_::ImGuiMouseButton_Right))
            {
                // select this item
                path.second = true;

                // show context menu
                draw_current_directory_popup_menu();
            }
            
            ImGui::PopID();

            // draw last write time
            ImGui::TableSetColumnIndex(1);

            try
            {
                auto time    = std::filesystem::last_write_time(path.first);
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
            ImGui::TextUnformatted((std::filesystem::is_directory(path.first) ? "folder" : "file"));
        }

        ImGui::EndTable();
    }
}

void Explorer::draw_current_directory_format_filter()
{
    std::string foramtFilterPreviewText;
    size_t      counter = 0;

    for (auto&& formatFilter : m_FormatFilter)
    {
        auto text = foramtFilterPreviewText;

        if(formatFilter.second)
        {
            text = text.append(" ").append(formatFilter.first);
            counter++;
        }

        if(ImGui::CalcTextSize(text.c_str()).x > ImGui::GetContentRegionAvail().x * 0.7f) 
        {
            foramtFilterPreviewText.append("...");
            break;
        }
        else
        {
            foramtFilterPreviewText = text;
        }
    }

    if(foramtFilterPreviewText.empty()) 
        foramtFilterPreviewText = "none";

    if (counter >= m_FormatFilter.size())
        foramtFilterPreviewText = "all";

    if(ImGui::BeginCombo("format filter", foramtFilterPreviewText.c_str()))
    {
        int checkboxID = 0;

        for (auto&& formatFilter : m_FormatFilter)
        {
            ImGui::PushID(checkboxID++);
            ImGui::Checkbox("##", &formatFilter.second);
            ImGui::SameLine();

            ImGui::Selectable(
                formatFilter.first.c_str(), 
                &formatFilter.second, 
                ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns | 
                ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowOverlap   | 
                ImGuiSelectableFlags_::ImGuiSelectableFlags_NoAutoClosePopups);

            ImGui::PopID();
        }

        ImGui::EndCombo();
    }
}

void Explorer::draw_current_directory_popup_menu()
{
    if(get_selected_paths().empty())
    {
        if(ImGui::BeginPopupContextItem())
        {
            FolderMenu().frame_update();
            ImGui::EndPopup();
        }
    }
    else
    {
        if(ImGui::BeginPopupContextItem())
        {
            FileMenu().frame_update();
            ImGui::EndPopup();
        }
    }
}

void Explorer::handle_current_directory_hot_keys()
{
    if(Application::instance()->find<Dialog>() != nullptr) 
        return;

    // Ctrl + C
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_C) && 
        (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightCtrl)))
    {            
        Frenchie::Application::Application::instance()->find_or_push<CommandsQueue>()->push(
            Frenchie::Application::Editor::FileSystem::FileMenuCopyAction::factory_id());
    }

    // Ctrl + V
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_V) && 
        (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightCtrl)))
    {            
        Frenchie::Application::Application::instance()->find_or_push<CommandsQueue>()->push(
            Frenchie::Application::Editor::FileSystem::FileMenuPasteAction::factory_id());
    }

    // Delete
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete))
    {            
        Frenchie::Application::Application::instance()->find_or_push<CommandsQueue>()->push(
            Frenchie::Application::Editor::FileSystem::FileMenuRemoveAction::factory_id());
    }

    // Escape
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
    {
        for(auto&& path : m_Paths)
            path.second = false;
    }
}

void Explorer::draw_paths_tree(const std::filesystem::path& _Path, int& _ID)
{
    size_t counter = 0;
    for(const auto& directory : 
        std::filesystem::directory_iterator(_Path, std::filesystem::directory_options::skip_permission_denied))
            counter++;

    if(counter <= 0) 
        return;

    if(_Path == std::filesystem::current_path().root_path())
        ImGui::SetNextItemOpen(true);

    ImGui::PushID(_ID++);

    if(ImGui::TreeNodeEx(pugi::as_utf8(_Path.filename().wstring()).c_str(), 
        ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_SpanAvailWidth | 
        ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DrawLinesFull))
    {
        change_current_directory(_Path);

        for(const auto& directory :
            std::filesystem::directory_iterator(_Path, std::filesystem::directory_options::skip_permission_denied))
        {
            if(directory.is_directory())
                draw_paths_tree(directory.path(), _ID);
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
};