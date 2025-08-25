#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

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
                class MenuCopyAction : 
                    public Frenchie::Core::Command::Registry<MenuCopyAction>
                {
                public:

                    // Frenchie::Core::Command
                    virtual void execute() override
                    {
                        auto explorer = Application::instance()->find<Explorer>();

                        if(explorer == nullptr || explorer->get_selected_paths().empty()) 
                            return;

                        auto selectedPaths = explorer->get_selected_paths();

                        if(selectedPaths.empty())
                            return;

                        // copy selected paths to cipboard
                        std::string clipBoardText;

                        for(auto& path : selectedPaths)
                        {
                            clipBoardText = clipBoardText.append(
                                fmt::format("{}\n", Frenchie::Core::Helpers::String::as_utf8(path.wstring()).c_str()));
                        }

                        ImGui::SetClipboardText(clipBoardText.c_str());
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "copy");
                    }
                };

                class MenuPasteAction : 
                    public Frenchie::Core::Command::Registry<MenuPasteAction>
                {
                public:

                    // Frenchie::Core::Command
                    virtual void execute() override
                    {
                        auto paths = Frenchie::Core::Helpers::String::split(std::string(ImGui::GetClipboardText()), "\n");

                        struct FileInfo
                        {
                            std::wstring name      = std::wstring();
                            std::string  extention = std::string();
                        };

                        auto getFileInfo = [](std::filesystem::path path, int _Depth = 2)->FileInfo
                        {
                            std::string extention = std::string();

                            for(int i = 0; i < _Depth; i++)
                            {
                                extention = extention.append(path.extension().string());
                                path = path.stem();
                            }

                            return
                            {
                                path.filename().wstring(),
                                extention

                            };
                        };

                        for(auto&& path : paths)
                        {
                            if(!std::filesystem::exists(path)) 
                                continue;

                            auto source = std::filesystem::path(path);

                            auto extention = Frenchie::Core::Helpers::get_file_extention(source);

                            auto target = std::filesystem::path(
                                source.parent_path().wstring()
                                .append(L"/")
                                .append(source.filename().stem().wstring())
                                .append(L"_Copy")
                                .append(pugi::as_wide(extention))).make_preferred();

                            Frenchie::Core::Logger::instance()->warn(source.string());
                            Frenchie::Core::Logger::instance()->warn(target.string());

                            std::filesystem::copy(source, target);
                        }
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "paste");
                    }
                };

                class MenuRemoveAction : 
                    public Frenchie::Core::Command::Registry<MenuRemoveAction>
                {
                public:

                    // Frenchie::Core::Command
                    virtual void execute() override
                    {
                        auto explorer = Application::instance()->find<Explorer>();

                        if(explorer == nullptr || explorer->get_selected_paths().empty()) 
                            return;

                        auto selectedPaths = explorer->get_selected_paths();

                        if(selectedPaths.empty())
                            return;

                        // remove selected paths
                        for(auto& path : selectedPaths)
                        {
                            try
                            {
                                std::filesystem::remove_all(path);
                            }
                            catch(...)
                            {

                            }
                        }
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu), "remove");
                    }
                };
            }
        }
    }
}

// FileSystemExplorerMenu
FileMenu::FileMenu(){}
FileMenu::~FileMenu(){}

void FileMenu::frame_update()
{
    m_MenuDrawer.draw(STRINGIFY(Frenchie::Application::Editor::FileSystem::FileMenu));
}

// FlileSystemExplorer
Explorer::Explorer() : 
    Layer(STRINGIFY(Explorer)){}

Explorer::~Explorer(){}

std::vector<std::filesystem::path> Explorer::get_selected_paths() const
{
    std::vector<std::filesystem::path> selectedPaths;
    
    for(auto&& path : m_Paths)
    {
        if(path.second) 
            selectedPaths.push_back(path.first);
    }

    return selectedPaths;
}

void Explorer::frame_update()
{
    // collect all files
    if(m_Paths.empty())
    {
        for(const auto& directory :
            std::filesystem::directory_iterator(std::filesystem::current_path().make_preferred(), 
            std::filesystem::directory_options::skip_permission_denied))
        {
            // get path
            auto path = directory.path();
            m_Paths.insert({path, false});

            // get extention
            auto extention = Frenchie::Core::Helpers::get_file_extention(path);

            if(!extention.empty()) 
            {
                m_FormatFilter[extention] = 
                    m_FormatFilter.find(extention) == m_FormatFilter.end() ? true : m_FormatFilter[extention];
            }
        }
    }

    // draw
    ImGui::Begin(get_name().c_str());
    {
        // draw current path editor
        if(m_CurrentDirectory.draw(
            Frenchie::Core::Helpers::String::as_utf8(std::filesystem::current_path().make_preferred().wstring()).c_str(), 
            ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
        {
            change_current_directory(std::filesystem::path(Frenchie::Core::Helpers::String::as_wide(m_CurrentDirectory.get_buffer())));
        }

        // draw table
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
                ImGui::SameLine();
                ImGui::TextUnformatted(Frenchie::Core::Helpers::String::as_utf8(path.first.filename().wstring()).c_str());
                ImGui::PopID();

                ImGui::SameLine();

                ImGui::PushID(widgetID++);

                bool selected = false;
                if(ImGui::Selectable(
                    "##", 
                    &selected,
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns    | 
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowOverlap      | 
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_NoAutoClosePopups |
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowDoubleClick))
                {
                    if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
                        change_current_directory(path.first);
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
        // draw menu
        if (ImGui::BeginPopupContextItem())
        {
            FileMenu().frame_update();
            ImGui::EndPopup();
        }

        // draw format filter
        if(ImGui::BeginCombo("format filter", "filter"))
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

        ImGui::End();
    }
}

void Explorer::change_current_directory(const std::filesystem::path& _Path)
{
    // change current path
    if(!std::filesystem::is_directory(_Path)) 
        return;

    Application::instance()
        ->find_or_push<Frenchie::Application::CommandsQueue>()
        ->push<Frenchie::Core::CallbackCommand>(
            [this, _Path]()
            {
                try
                {
                    auto path = _Path;

                    // change current path
                    std::filesystem::current_path(path.make_preferred());

                    // clear selected files
                    m_Paths.clear();
                    m_FormatFilter.clear();
                }
                catch(const std::exception& e)
                {
                    Frenchie::Core::Logger::instance()->critical(e.what());
                }
            }
        );
}