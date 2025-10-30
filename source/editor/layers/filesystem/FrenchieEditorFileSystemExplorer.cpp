#include <FrenchieEditorFileSystemExplorer.hpp>

// Core
#include <FrenchieCoreThreadPool.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>
#include <FrenchieApplicationEditorDialog.hpp>

// Editor
#include <FrenchieEditorHelpers.hpp>
#include <FrenchieCoreFileSystem.hpp>
#include <FrenchieCoreStringUtilities.hpp>

using namespace Frenchie::Core;

using namespace Frenchie::Application;
using namespace Frenchie::Application::Configuration;

using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;
using namespace Frenchie::Application;

// Add to main menu
namespace Frenchie
{
    namespace Editor
    {
        namespace MainMenu
        {
            class OpenFileSystemExplorerAction : 
                public Frenchie::Application::Command::Registry<OpenFileSystemExplorerAction, const std::shared_ptr<CommandPayload>&>
            {
            public:

                OpenFileSystemExplorerAction(const std::shared_ptr<CommandPayload>& _Sender = nullptr) : 
                    Frenchie::Application::Command::Registry<OpenFileSystemExplorerAction, const std::shared_ptr<CommandPayload>&>(_Sender){}
                virtual ~OpenFileSystemExplorerAction(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->push_layer<Frenchie::Editor::FileSystem::Explorer>()->show();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Windows::FileSystem");
                }
            };
        }
    }
}

namespace Frenchie
{
    namespace Editor
    {
        namespace FileSystem
        {
            class ExplorerPayload : public Frenchie::Application::CommandPayload
            {
            public:
                ExplorerPayload(Explorer* _Explorer) : m_Explorer(_Explorer){}
                virtual ~ExplorerPayload(){}

                Explorer* m_Explorer;
            };

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
                        Frenchie::Editor::Helpers::draw_menu(
                            STRINGIFY(Frenchie::Editor::FileSystem::FolderMenu),
                            std::make_shared<ExplorerPayload>(_Explorer));
                    }
                };

                class CreateFolderAction : 
                    public Command::Registry<CreateFolderAction, const std::shared_ptr<CommandPayload>&>
                {
                public:

                    CreateFolderAction(const std::shared_ptr<CommandPayload>& _Payload) : 
                        Command::Registry<CreateFolderAction, const std::shared_ptr<CommandPayload>&>(_Payload){}
                    virtual ~CreateFolderAction(){}

                    // Frenchie::Application::Command
                    virtual void execute() override
                    {
                        get_payload<ExplorerPayload>()->m_Explorer->create_folder();
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::FolderMenu), "Create::Create folder");
                    }
                };

                class PasteAction : 
                    public Command::Registry<PasteAction, const std::shared_ptr<CommandPayload>&>
                {
                public:
                    PasteAction(const std::shared_ptr<CommandPayload>& _Payload) : 
                        Command::Registry<PasteAction, const std::shared_ptr<CommandPayload>&>(_Payload){}
                    virtual ~PasteAction(){}

                    // Frenchie::Application::Command
                    virtual void execute() override
                    {
                        get_payload<ExplorerPayload>()->m_Explorer->paste_paths();
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::FolderMenu), "Paste");
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
                        Frenchie::Editor::Helpers::draw_menu(
                            STRINGIFY(Frenchie::Editor::FileSystem::FileMenu),
                            std::make_shared<ExplorerPayload>(_Explorer));
                    }
                };

                class CopyAction : 
                    public Command::Registry<CopyAction, const std::shared_ptr<CommandPayload>&>
                {
                public:
                    CopyAction(const std::shared_ptr<CommandPayload>& _Payload) : 
                        Command::Registry<CopyAction, const std::shared_ptr<CommandPayload>&>(_Payload){}
                    virtual ~CopyAction(){}

                    // Frenchie::Application::Command
                    virtual void execute() override
                    {
                        get_payload<ExplorerPayload>()->m_Explorer->copy_paths();
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::FileMenu), "Copy");
                    }
                };

                class PasteAction : 
                    public Command::Registry<PasteAction, const std::shared_ptr<CommandPayload>&>
                {
                public:
                    PasteAction(const std::shared_ptr<CommandPayload>& _Payload) : 
                        Command::Registry<PasteAction, const std::shared_ptr<CommandPayload>&>(_Payload){}
                    virtual ~PasteAction(){}

                    virtual void execute() override
                    {
                        get_payload<ExplorerPayload>()->m_Explorer->paste_paths();
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::FileMenu), "Paste");
                    }
                };

                class RemoveAction : 
                    public Command::Registry<RemoveAction, const std::shared_ptr<CommandPayload>&>
                {
                public:
                    RemoveAction(const std::shared_ptr<CommandPayload>& _Payload) : 
                        Command::Registry<RemoveAction, const std::shared_ptr<CommandPayload>&>(_Payload){}
                    virtual ~RemoveAction(){}

                    // Frenchie::Application::Command
                    virtual void execute() override
                    {
                        get_payload<ExplorerPayload>()->m_Explorer->remove_paths();
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::FileMenu), "Remove");
                    }
                };

                class RenameAction : 
                    public Command::Registry<RenameAction, const std::shared_ptr<CommandPayload>&>
                {
                public:

                    RenameAction(const std::shared_ptr<CommandPayload>& _Payload) : 
                        Command::Registry<RenameAction, const std::shared_ptr<CommandPayload>&>(_Payload){}
                    virtual ~RenameAction(){}

                    // Frenchie::Application::Command
                    virtual void execute() override
                    {
                        get_payload<ExplorerPayload>()->m_Explorer->rename_paths();
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::FileMenu), "Rename");
                    }
                };

                class CreateFolderAction : 
                    public Command::Registry<CreateFolderAction, const std::shared_ptr<CommandPayload>&>
                {
                public:
                    CreateFolderAction(const std::shared_ptr<CommandPayload>& _Payload) : 
                        Command::Registry<CreateFolderAction, const std::shared_ptr<CommandPayload>&>(_Payload){}
                    virtual ~CreateFolderAction(){}

                    // Frenchie::Application::Command
                    virtual void execute() override
                    {
                        get_payload<ExplorerPayload>()->m_Explorer->create_folder();
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::FileMenu), "Create::Create folder");
                    }
                };
            }
        
            namespace Dialogs
            {
                class RemoveFiles : public Dialog
                {
                public:
                    RemoveFiles(const std::set<std::filesystem::path>& _Paths) : 
                        Dialog(Translator::translate("Are you sure you want to remove these files ?")), 
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
                        if(ImGui::Button(Translator::translate("Yes").c_str()))
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
                        
                        if(ImGui::Button(Translator::translate("No").c_str()))
                            close();
                    }

                protected:
                    std::set<std::filesystem::path> m_Paths;
                };
            
                class RenameFiles : public Dialog
                {
                public:

                    RenameFiles(const std::set<std::filesystem::path>& _Paths) : 
                        Dialog(Translator::translate("Rename files"))
                    {
                        for(auto&& path : _Paths)
                        {
                            m_Paths.insert(
                                {
                                    path, 
                                    {
                                        Frenchie::Core::String::convert_utf32_to_utf8(path.filename().u32string()), 
                                        true
                                    }
                                }
                            );
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
                            ImGui::InputText("###", &path.second.first);

                            ImGui::PopID();
                        }
                    }

                    virtual void draw_buttons() override
                    {
                            if(ImGui::Button(Translator::translate("Ok").c_str()))
                            {
                                // rename files
                                for(auto& item : m_Paths)
                                {
                                    auto source   = item.first;
                                    auto& text    = item.second.first;
                                    auto selected = item.second.second;

                                    if(!std::filesystem::exists(source) || !selected) 
                                        continue;

                                    auto target = std::filesystem::path(
                                        source.parent_path().u32string()
                                        .append(U"/")
                                        .append(Frenchie::Core::String::convert_utf8_to_utf32(text))
                                    );

                                    if(source == target) 
                                        continue;

                                    while (std::filesystem::exists(target))
                                    {
                                        auto extention = Frenchie::Core::String::convert_utf8_to_utf32(
                                            Frenchie::Core::FileSystem::get_file_extention(target));

                                        target = source.parent_path().u32string()
                                            .append(U"/")
                                            .append(target.filename().stem().u32string())
                                            .append(U"_Copy")
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
                            
                            if(ImGui::Button(Translator::translate("Cancel").c_str()))
                                close();
                    }

                protected:
                    std::map<
                        std::filesystem::path, 
                        std::pair<std::string, bool>> m_Paths;
                };

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

std::filesystem::path Explorer::get_current_path() const
{
    return m_Path;
}

std::filesystem::path Explorer::get_current_file() const
{
    return std::filesystem::path(
        fmt::format("{}/{}", get_current_path().string(), m_Selection.m_CurrentFile)).make_preferred();
}

std::set<std::filesystem::path> Explorer::get_selected_paths() const
{
    return m_Selection.get_selected_paths();
}

void Explorer::create_folder()
{
    std::u32string newFolderName = U"NewFolder";

    auto newPath = m_Path.u32string().append(U"/").append(newFolderName);

    while(std::filesystem::exists(newPath))
        newPath = newPath.append(U"_Copy");

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
            m_Path.u32string()
            .append(U"/")
            .append(source.filename().stem().u32string())
            .append(U"_Copy")
            .append(Frenchie::Core::String::convert_utf8_to_utf32(extention))).make_preferred();

        while(std::filesystem::exists(target))
        {
            target = std::filesystem::path(
                m_Path.u32string()
                .append(U"/")
                .append(target.filename().stem().u32string())
                .append(U"_Copy")
                .append(Frenchie::Core::String::convert_utf8_to_utf32(extention))).make_preferred();
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
        application()->push_layer<Dialogs::RemoveFiles>(selectedPaths);
}

void Explorer::rename_paths()
{
    auto selectedPaths = get_selected_paths();

    if(!selectedPaths.empty()) 
        application()->push_layer<Dialogs::RenameFiles>(selectedPaths);
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

    Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
            [this, _Path]()
            {
                try
                {
                    // change current path
                    auto path = _Path;

                    m_Path = path.make_preferred();

                    // clear selection
                    m_Selection.clear_selection();
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
            auto name = Frenchie::Core::String::convert_utf32_to_utf8(path.filename().u32string());

            if(name.empty()) 
                name = Frenchie::Core::String::convert_utf32_to_utf8(path.u32string());

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
            m_CurrentDirectory = Frenchie::Core::String::convert_utf32_to_utf8(m_Path.make_preferred().u32string());

        if(ImGui::InputText(
            Translator::translate("Current directory").c_str(), 
                &m_CurrentDirectory, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
        {            
            change_current_directory(
                std::filesystem::path(Frenchie::Core::String::convert_utf8_to_utf32(m_CurrentDirectory)));

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
    ImGui::InputText(
        Translator::translate("Current file").c_str(), 
            &m_Selection.m_CurrentFile, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue);
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
            1,
            ImGuiTableFlags_::ImGuiTableFlags_ScrollY      | 
            ImGuiTableFlags_::ImGuiTableFlags_RowBg        | 
            ImGuiTableFlags_::ImGuiTableFlags_BordersOuter | 
            ImGuiTableFlags_::ImGuiTableFlags_BordersV     |
            ImGuiTableFlags_::ImGuiTableFlags_Resizable    |
            ImGuiTableFlags_::ImGuiTableFlags_Reorderable  |
            ImGuiTableFlags_::ImGuiTableFlags_Hideable, 
            ImVec2(0.0, ImGui::GetContentRegionAvail().y - 2.0f * ImGui::GetTextLineHeightWithSpacing())))
    {
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

                bool selected = m_Selection.contains(path);

                if(ImGui::Selectable(
                    Frenchie::Core::String::convert_utf32_to_utf8(path.filename().u32string()).c_str(), 
                    &selected,
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns    | 
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowOverlap      | 
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_NoAutoClosePopups |
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowDoubleClick))
                {
                    if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) || 
                        ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightCtrl))
                    {
                        m_Selection.select_path(path);
                    }
                    else
                    {
                        m_Selection.clear_selection();
                        m_Selection.select_path(path);
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
                    m_Selection.select_path(path);

                    // show context menu
                    draw_current_directory_popup_menu();
                }
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
    if(application()->find_layer<Dialog>() != nullptr) 
        return;

    // Ctrl + C
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_C) && 
        (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightCtrl)))
    {            
        Frenchie::Application::CommandsQueue::instance()->push(
            FileMenu::CopyAction::factory_id(),
            std::make_shared<ExplorerPayload>(this));
    }

    // Ctrl + V
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_V) && 
        (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightCtrl)))
    {            
        Frenchie::Application::CommandsQueue::instance()->push(FileMenu::PasteAction::factory_id(), std::make_shared<ExplorerPayload>(this));
    }

    // Delete
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete))
    {            
        Frenchie::Application::CommandsQueue::instance()->push(FileMenu::RemoveAction::factory_id(), std::make_shared<ExplorerPayload>(this));
    }

    // Escape
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
    {
        m_Selection.clear_selection();
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

    auto name = _Path.filename().u32string();

    drag_selected_paths(_Path);

    if(ImGui::TreeNodeEx(Frenchie::Core::String::convert_utf32_to_utf8(name.empty() ? U"Root" : name).c_str(), 
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

            selectionBuffer.append(Frenchie::Core::String::convert_utf32_to_utf8(selectedPath.u32string())).append(";");
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
            
            auto movedPaths = Frenchie::Core::String::split_utf8_string(std::string(adress), ";");

            for(auto&& movedPath : movedPaths)
            {
                std::filesystem::path oldAdress(Frenchie::Core::String::convert_utf8_to_utf32(movedPath));

                if(!std::filesystem::exists(oldAdress)) 
                    continue;

                std::filesystem::path newAdress(_Path.u32string().append(U"/").append(oldAdress.filename().u32string()));

                if(oldAdress.parent_path() == newAdress.parent_path()) 
                    return;

                while(std::filesystem::exists(newAdress))
                {
                    newAdress = 
                        _Path.u32string()
                        .append(U"/")
                        .append(newAdress.filename().stem().u32string())
                        .append(U"_Copy")
                        .append(Frenchie::Core::String::convert_utf8_to_utf32(Frenchie::Core::FileSystem::get_file_extention(oldAdress)));
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