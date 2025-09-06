#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

#include <FrenchieApplication.hpp>
#include <FrenchieApplicationEditorMenu.hpp>
#include <FrenchieApplicationEditorCommandsLayer.hpp>
#include <FrenchieApplicationEditorAbstractDialogLayer.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;
using namespace Frenchie::Application::Editor::FileSystem;

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            namespace FileSystem
            {
                namespace FolderMenu
                {
                    class Instance
                    {
                    public:
                        Instance(){}
                        virtual ~Instance(){}

                        // API
                        void draw(Explorer* _Explorer)
                        {
                            Menu().draw(STRINGIFY(Frenchie::Application::Editor::FileSystem::FolderMenu), _Explorer);
                        }
                    };

                    class CreateFolderAction : 
                        public Frenchie::Application::Command::Registry<CreateFolderAction, void*>
                    {
                    public:

                        CreateFolderAction(void* _Sender) : 
                            Frenchie::Application::Command::Registry<CreateFolderAction, void*>(_Sender){}
                        virtual ~CreateFolderAction(){}

                        // Frenchie::Application::Command
                        virtual void execute() override
                        {
                            get_sender<Explorer>()->create_folder();
                        }


                        // Command::TRegistryType
                        static std::string factory_id()
                        {
                            return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FolderMenu), "create::folder");
                        }

                    };

                    class PasteAction : 
                        public Frenchie::Application::Command::Registry<PasteAction, void*>
                    {
                    public:
                        PasteAction(void* _Sender) : 
                            Frenchie::Application::Command::Registry<PasteAction, void*>(_Sender){}
                        virtual ~PasteAction(){}

                        // Frenchie::Application::Command
                        virtual void execute() override
                        {
                            get_sender<Explorer>()->paste_paths();
                        }

                        // Command::TRegistryType
                        static std::string factory_id()
                        {
                            return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FolderMenu), "paste");
                        }
                    };
                }
            
                namespace FileMenu
                {
                    class Instnance
                    {
                    public:
                        Instnance(){}
                        virtual ~Instnance(){}

                        // API
                        void draw(Explorer* _Explorer)
                        {
                            Menu().draw(STRINGIFY(Frenchie::Application::Editor::FileMenu), _Explorer);
                        }
                    };

                    class CopyAction : 
                        public Frenchie::Application::Command::Registry<CopyAction, void*>
                    {
                    public:
                        CopyAction(void* _Sender = nullptr) : 
                            Frenchie::Application::Command::Registry<CopyAction, void*>(_Sender){}
                        virtual ~CopyAction(){}

                        // Frenchie::Application::Command
                        virtual void execute() override
                        {
                            get_sender<Explorer>()->copy_paths();
                        }

                        // Command::TRegistryType
                        static std::string factory_id()
                        {
                            return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileMenu), "copy");
                        }
                    };

                    class PasteAction : 
                        public Frenchie::Application::Command::Registry<PasteAction, void*>
                    {
                    public:
                        PasteAction(void* _Sender = nullptr) : 
                            Frenchie::Application::Command::Registry<PasteAction, void*>(_Sender){}
                        virtual ~PasteAction(){}

                        virtual void execute() override
                        {
                            get_sender<Explorer>()->paste_paths();
                        }

                        // Command::TRegistryType
                        static std::string factory_id()
                        {
                            return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileMenu), "paste");
                        }
                    };

                    class RemoveAction : 
                        public Frenchie::Application::Command::Registry<RemoveAction, void*>
                    {
                    public:
                        RemoveAction(void* _Sender = nullptr) : 
                            Frenchie::Application::Command::Registry<RemoveAction, void*>(_Sender){}
                        virtual ~RemoveAction(){}

                        // Frenchie::Application::Command
                        virtual void execute() override
                        {
                            get_sender<Explorer>()->remove_paths();
                        }

                        // Command::TRegistryType
                        static std::string factory_id()
                        {
                            return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileMenu), "remove");
                        }
                    };

                    class RenameAction : 
                        public Frenchie::Application::Command::Registry<RenameAction, void*>
                    {
                    public:

                        RenameAction(void* _Sender = nullptr) : 
                            Frenchie::Application::Command::Registry<RenameAction, void*>(_Sender){}
                        virtual ~RenameAction(){}

                        // Frenchie::Application::Command
                        virtual void execute() override
                        {
                            get_sender<Explorer>()->rename_paths();
                        }

                        // Command::TRegistryType
                        static std::string factory_id()
                        {
                            return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileMenu), "rename");
                        }
                    };

                    class CreateFolderAction : 
                        public Frenchie::Application::Command::Registry<CreateFolderAction, void*>
                    {
                    public:
                        CreateFolderAction(void* _Sender = nullptr) : 
                            Frenchie::Application::Command::Registry<CreateFolderAction, void*>(_Sender){}
                        virtual ~CreateFolderAction(){}

                        // Frenchie::Application::Command
                        virtual void execute() override
                        {
                            get_sender<Explorer>()->create_folder();
                        }

                        // Command::TRegistryType
                        static std::string factory_id()
                        {
                            return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileMenu), "create::folder");
                        }
                    };
                }
            
                namespace Dialogs
                {
                    class RemoveFiles : public Dialog
                    {
                    public:
                        RemoveFiles(const std::set<std::filesystem::path>& _Paths) : 
                            Dialog("Are you sure you want to delete these files ?"), 
                            m_Paths(_Paths){}
                        virtual ~RemoveFiles(){}

                        // Frenchie::Application::Editor::Dialog
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
                        std::set<std::filesystem::path> m_Paths;
                    };
                
                    class RenameFiles : public Dialog
                    {
                    public:

                        RenameFiles(const std::set<std::filesystem::path>& _Paths) : 
                            Dialog("Rename files")
                        {
                            for(auto&& path : _Paths)
                            {
                                auto textInput = std::make_shared<InputText>();
                                textInput->set_buffer(Frenchie::Core::String::as_utf8(path.filename().wstring()));
                                m_Paths.insert({path, {textInput, true}});
                            }
                        }

                        virtual ~RenameFiles(){}

                        // Frenchie::Application::Editor::Dialog
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
                                path.second.first->draw("###");

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
                                            .append(Frenchie::Core::String::as_wide(text))
                                        );

                                        if(source == target) 
                                            continue;

                                        while (std::filesystem::exists(target))
                                        {
                                            auto extention = Frenchie::Core::String::as_wide(
                                                Frenchie::Core::FileSystem::get_file_extention(target));

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
                }
            }
        }
    }
}

// Explorer
Explorer::Explorer(
    const std::string&           _Name,
    const std::filesystem::path& _Path) : 
    Layer(_Name), 
    m_Path((std::filesystem::exists(_Path) ? _Path : std::filesystem::current_path())){}

Explorer::~Explorer(){}

std::filesystem::path Explorer::get_path() const
{
    return m_Path;
}

std::set<std::filesystem::path> Explorer::get_selected_paths() const
{    
    auto selection = m_SelectedPaths;

    for(auto&& path : selection)
    {
        if(!std::filesystem::exists(path))
            m_SelectedPaths.erase(path);
    }

    return m_SelectedPaths;
}

void Explorer::create_folder()
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

void Explorer::copy_paths()
{
    m_CopiedPaths = get_selected_paths();
}

void Explorer::paste_paths()
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

void Explorer::remove_paths()
{
    auto selectedPaths = get_selected_paths();

    if(!selectedPaths.empty()) 
        Application::instance()->push<Dialogs::RemoveFiles>(selectedPaths);
}

void Explorer::rename_paths()
{
    auto selectedPaths = get_selected_paths();

    if(!selectedPaths.empty()) 
        Application::instance()->push<Dialogs::RenameFiles>(selectedPaths);
}

void Explorer::frame_update()
{
    // draw content
    ImGui::Begin(get_name().c_str(), &m_Shown);
    {
        draw_contents();
        ImGui::End();
    }
}

void Explorer::change_current_directory(const std::filesystem::path& _Path)
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

void Explorer::draw_contents()
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

void Explorer::draw_current_filename_editor()
{
    m_CurrentFile.set_buffer(
        m_SelectedPaths.empty() ? "No file selected..." : Frenchie::Core::String::as_utf8((*m_SelectedPaths.begin()).filename().wstring()));

    m_CurrentFile.draw("CurrentFile");
}

void Explorer::draw_current_directory_paths_table()
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

void Explorer::draw_current_directory_popup_menu()
{
    if(ImGui::BeginPopupContextItem())
    {
        if(get_selected_paths().empty())
            FolderMenu::Instance().draw(this);
        else 
            FileMenu::Instnance().draw(this);
        
        ImGui::EndPopup();
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
            FileMenu::CopyAction::factory_id(), this);
    }

    // Ctrl + V
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_V) && 
        (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightCtrl)))
    {            
        Frenchie::Application::Application::instance()->find_or_push<CommandsQueue>()->push(
            FileMenu::PasteAction::factory_id(), this);
    }

    // Delete
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete))
    {            
        Frenchie::Application::Application::instance()->find_or_push<CommandsQueue>()->push(
            FileMenu::RemoveAction::factory_id(), this);
    }

    // Escape
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
    {
        m_SelectedPaths.clear();
    }
}

void Explorer::draw_paths_tree(const std::filesystem::path& _Path)
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

void Explorer::drag_selected_paths(const std::filesystem::path& _Path)
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

void Explorer::drop_path_to(const std::filesystem::path& _Path)
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

// OpenFilesDialog
Dialogs::GetOpenFiles::GetOpenFiles(const std::function<void()>& _OnAccpected, const std::string& _Name) : 
    FileSystem::Explorer(_Name, std::filesystem::current_path()), 
    m_OnAccepted(_OnAccpected){}

Dialogs::GetOpenFiles::~GetOpenFiles(){}

void Dialogs::GetOpenFiles::frame_update()
{
    ImGui::OpenPopup(m_Name.c_str());

    ImGuiStyle& style = ImGui::GetStyle();

    // press button
    auto wiondowFlags =
        ImGuiWindowFlags_::ImGuiWindowFlags_None        | 
        ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | 
        ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::SetNextWindowSizeConstraints(ImVec2(512, 512), ImVec2(FLT_MAX, FLT_MAX));

    if(ImGui::BeginPopupModal(m_Name.c_str(), &m_Opened))
    {
        ImGui::BeginChild(
            "Content",
            ImVec2(
                ImGui::GetContentRegionAvail().x, 
                ImGui::GetContentRegionAvail().y - (ImGui::CalcTextSize("Button").y + style.FramePadding.x * 2.0f + ImGui::CalcTextSize("BUTTON").y)),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            wiondowFlags);
        {
            draw_contents();
            ImGui::EndChild();
        }

        ImGui::BeginChild(
            "Buttons",
            ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            wiondowFlags);

        {
            if(ImGui::Button("Ok"))
            {
                if(m_OnAccepted != nullptr) 
                    m_OnAccepted();
                close();
            }

             ImGui::SameLine();

            if(ImGui::Button("Cancel"))
            {
                close();
            }

            ImGui::EndChild();
        }

        ImGui::EndPopup();
    }
}

bool Dialogs::GetOpenFiles::allows_multiple_instances() const
{
    return false;
}

Dialogs::PathScannerModel::PathScannerModel(
    const std::filesystem::path&                                       _Path,
    const std::function<bool(const std::filesystem::path&)>&           _Predicate,
    const std::function<bool(const std::set<std::filesystem::path>&)>& _OnFinished,
    size_t                                                             _MaxSearchDepth) : 
    m_Path(_Path),
    m_Predicate(_Predicate),
    m_OnFinished(_OnFinished),
    m_MaxSearchDepth(_MaxSearchDepth){}

Dialogs::PathScannerModel::~PathScannerModel(){}

std::set<std::filesystem::path>& Dialogs::PathScannerModel::get_paths()
{
    return m_Paths;
}

bool Dialogs::PathScannerModel::awake()
{
    if(!std::filesystem::exists(m_Path) ||
        m_Predicate == nullptr) 
        return false;

    try
    {
        m_Iterator = std::filesystem::recursive_directory_iterator(m_Path);
    }
    catch(const std::exception& e)
    {
        Frenchie::Core::Logger::instance()->critical(e.what());
        return false;
    }

    return m_Iterator != std::filesystem::recursive_directory_iterator();
}

std::string Dialogs::PathScannerModel::execute()
{
    try 
    {
        auto path = m_Iterator->path();

        if (m_Iterator.depth() > m_MaxSearchDepth) 
            const_cast<std::filesystem::recursive_directory_iterator&>(m_Iterator).disable_recursion_pending();
        
        if(m_Predicate(path))
            m_Paths.insert(path);

        m_Iterator++; // go ahead

        return Frenchie::Core::String::as_utf8(path);
    } 
    catch (const std::exception& e) 
    {
        Frenchie::Core::Logger::instance()->error(e.what());
        return std::string();
    }
}

void Dialogs::PathScannerModel::finish()
{
    if(m_OnFinished != nullptr) 
        m_OnFinished(m_Paths);
}

bool Dialogs::PathScannerModel::finished()
{
    return m_Iterator == std::filesystem::recursive_directory_iterator();
}

// PathScannerView
Dialogs::PathScannerView::PathScannerView(std::shared_ptr<PathScannerModel> _Model, const std::string& _Name) : 
    Dialog(_Name, 512.f, 128.f),
    m_Model(_Model){}

Dialogs::PathScannerView::~PathScannerView(){}

bool Dialogs::PathScannerView::awake()
{
    return m_Model != nullptr && m_Model->awake();
}

void Dialogs::PathScannerView::draw_content()
{
    if(m_Model == nullptr || m_Model->finished()) 
    {
        close();
        return;
    }

    // calculate progress percantage
    std::string text = m_Model->execute();

    // show progress
    ImGui::Text("Scanning %s", text.c_str());
}

void Dialogs::PathScannerView::draw_buttons()
{
    if(ImGui::Button("Cancel")) 
    {
        m_Model = nullptr;
        close();
    }
}

void Dialogs::PathScannerView::finish()
{
    if(m_Model != nullptr) 
        m_Model->finish();
}