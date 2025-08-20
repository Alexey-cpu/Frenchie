#include <FrenchieApplicationEditorConsoleLayer.hpp>

// SPDLOG
#include <spdlog/sinks/base_sink.h>
#include <spdlog/details/null_mutex.h>

// IMGUI
#include <imgui.h>
#include <imgui_internal.h>

// STL
#include <mutex>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class ConsoleSink : public spdlog::sinks::base_sink<std::mutex>
            {
            public:
                ConsoleSink(const Console* _Console) : m_Console(_Console){}
                virtual ~ConsoleSink(){}

            protected:

                virtual void sink_it_(const spdlog::details::log_msg& _Message) override
                {
                    if (m_Console == nullptr || 
                            m_Console->m_Messages.size() >= m_Console->m_MaximumMessageCount) 
                            return;

                    // create message
                    auto message = m_Console->m_Messages.append_node("message");
                    message.append_node("time", Helpers::String::to_string<std::chrono::system_clock::time_point>(_Message.time).c_str(), Frenchie::Core::Serialization::NodeType::OBJECT);
                    message.append_node("level", ConsoleSink::get_level(_Message.level).c_str());
                    message.append_node("level_enum", Helpers::String::to_string<size_t>(_Message.level).c_str());
                    message.append_node("message", fmt::to_string(_Message.payload).c_str());
                    message.append_node("logger", fmt::to_string(_Message.logger_name).c_str());

                    message.append_node("selected", false);

                    message.append_node("color", Helpers::String::to_string<ImU32>(ConsoleSink::get_color(_Message.level)).c_str(), Frenchie::Core::Serialization::NodeType::OBJECT);
                }

                virtual void flush_() override{}

            private:

                // info
                const Console* m_Console = nullptr;

                // service methods
                static ImU32 get_color(spdlog::level::level_enum _Level)
                {
                    switch (_Level)
                    {
                        case spdlog::level::level_enum::trace:
                            return IM_COL32(128, 128, 128, 255);

                        case spdlog::level::level_enum::debug:
                            return IM_COL32(200, 200, 200, 255);

                        case spdlog::level::level_enum::info:
                            return IM_COL32(0, 200, 0, 255);

                        case spdlog::level::level_enum::warn:
                            return IM_COL32(233, 245, 66, 255);

                        case spdlog::level::level_enum::err:
                            return IM_COL32(255, 0, 0, 255);

                        case spdlog::level::level_enum::critical:
                            return IM_COL32(255, 255, 255, 255);
                    }

                    return IM_COL32(255, 255, 255, 255);
                }

                static std::string get_level(spdlog::level::level_enum _Level)
                {
                    switch (_Level)
                    {
                        case spdlog::level::level_enum::trace:
                            return "trace";

                        case spdlog::level::level_enum::debug:
                            return "debug";

                        case spdlog::level::level_enum::info:
                            return "info";

                        case spdlog::level::level_enum::warn:
                            return "warning";

                        case spdlog::level::level_enum::err:
                            return "error";

                        case spdlog::level::level_enum::critical:
                            return "critical";
                    }

                    return "unknown";
                }
            };
        }
    }
}

// Console
Console::Console() : 
    Layer::Registry<Console>(STRINGIFY(Frenchie::Application::Editor::Console))
{
    m_MessageTypeFilter.resize(spdlog::level::level_enum::n_levels - 1);
    m_MessageTypeFilter[spdlog::level::level_enum::trace]    = {"trace",    false};
    m_MessageTypeFilter[spdlog::level::level_enum::debug]    = {"debug",    false};
    m_MessageTypeFilter[spdlog::level::level_enum::info]     = {"info",     true};
    m_MessageTypeFilter[spdlog::level::level_enum::warn]     = {"warning",  true};
    m_MessageTypeFilter[spdlog::level::level_enum::err]      = {"error",    true};
    m_MessageTypeFilter[spdlog::level::level_enum::critical] = {"critical", true};
}

// virtual destructor
Console::~Console(){}

// Layer
bool Console::awake()
{
    Frenchie::Core::Logger::instance()->register_sink<ConsoleSink>(this);

    return true;
}

void Console::frame_start()
{
    Layer::frame_start();
}

void Console::frame_update()
{
    // handle events
    if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
    {
        for(auto&& message : m_Messages) 
            message.find_node("selected").set_value_as<bool>(false);
    }

    // create dockspace
    char consoleTableID[] = "Protocol";
    char consoleTextID [] = "Selection";

    auto dockspace_id = ImGui::GetID(get_name().c_str());

    if(ImGui::Begin(get_name().c_str(), &m_Opened))
    {
        ImGuiDockNodeFlags dockspaceFlags = 
            ImGuiDockNodeFlags_::ImGuiDockNodeFlags_PassthruCentralNode | 
            ImGuiDockNodeFlags_::ImGuiDockNodeFlags_NoUndocking;

        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);

        if(m_InitializeDockSpace)
        {
            m_InitializeDockSpace = false;
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, dockspaceFlags);
            ImGui::DockBuilderDockWindow(consoleTableID, ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.8f, nullptr, &dockspace_id));
            ImGui::DockBuilderDockWindow(consoleTextID, ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.2f, nullptr, &dockspace_id));
            ImGui::DockBuilderFinish(dockspace_id);
        }

        ImGui::End();
    }

    // docked windows
    ImGui::Begin(consoleTableID);
    {
        // debugging buttons
        if(ImGui::Button("Trace")) Frenchie::Core::Logger::instance()->trace("trace");
        if(ImGui::Button("Debug")) Frenchie::Core::Logger::instance()->debug("debug");
        if(ImGui::Button("Info")) Frenchie::Core::Logger::instance()->info("info");
        if(ImGui::Button("Warn")) Frenchie::Core::Logger::instance()->warn("warn");
        if(ImGui::Button("Error")) Frenchie::Core::Logger::instance()->error("error");
        if(ImGui::Button("Critical")) Frenchie::Core::Logger::instance()->critical("critical");

        // draw clear button
        if(ImGui::Button("Clear"))
            m_Messages.reset();

        // draw message type filter
        for (int i = 0; i < (int)m_MessageTypeFilter.size(); i++)
        {
            ImGui::SameLine();
            ImGui::Checkbox(m_MessageTypeFilter[i].Level.c_str(), &m_MessageTypeFilter[i].Selected);
        }

        ImGui::SameLine();

        // draw message text filter
        auto textFilterLabel = "Text fiter";

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(textFilterLabel).x * 2.f);

        ImGui::InputText(textFilterLabel, 
            m_MessageContentFilter,  
            sizeof(m_MessageContentFilter) / sizeof(m_MessageContentFilter[0]));

        // draw message table
        //if(ImGui::BeginChild("MessagesTable", ImVec2(0.f, 0.f), ImGuiChildFlags_::ImGuiChildFlags_Borders))
        ImGui::BeginChild("MessagesTable", ImVec2(0.f, 0.f), ImGuiChildFlags_::ImGuiChildFlags_Borders);
        {
            if(ImGui::BeginTable(
                "Messages", 
                4, 
                ImGuiTableFlags_::ImGuiTableFlags_Borders   | 
                ImGuiTableFlags_::ImGuiTableFlags_Resizable | 
                ImGuiTableFlags_::ImGuiTableFlags_Reorderable))
            {
                ImGui::TableSetupColumn("level");
                ImGui::TableSetupColumn("time");
                ImGui::TableSetupColumn("logger");
                ImGui::TableSetupColumn("message");
                ImGui::TableHeadersRow();

                // draw a content of spdlog buffer here
                int id = 0;

                for(auto&& message : m_Messages)
                {
                    // check message type filter 
                    if(!m_MessageTypeFilter[message.find_node("level_enum").get_value_as<size_t>()].Selected) 
                        continue;

                    // check message text filter
                    std::string messageText       = message.find_node("message").get_value();
                    std::string messageTextFilter = m_MessageContentFilter;

                    auto iterator = std::search(
                        messageText.begin(), 
                        messageText.end(), 
                        std::boyer_moore_searcher(messageTextFilter.begin(), messageTextFilter.end()));

                    if(!messageTextFilter.empty() && iterator == messageText.end()) 
                        continue;

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    ImGui::PushID(id);  
                    
                    bool selected = message.find_node("selected").get_value_as<bool>();

                    if(ImGui::Selectable("", 
                        &selected, 
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns | 
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowOverlap))
                    {
                        message.find_node("selected").set_value_as<bool>(true);
                    }

                    ImGui::PopID();

                    ImGui::SameLine();

                    ImGui::PushStyleColor(ImGuiCol_Text, message.find_node("color").get_value_as<ImU32>());
                    
                    ImGui::TextUnformatted(message.find_node("level").get_value());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(message.find_node("time").get_value());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextUnformatted(message.find_node("logger").get_value());

                    ImGui::TableSetColumnIndex(3);
                    ImGui::TextUnformatted(message.find_node("message").get_value());

                    ImGui::PopStyleColor();

                    id++;
                }

                ImGui::EndTable();
            }

            ImGui::EndChild();
        }

        ImGui::End();
    }

    // draw selection
    ImGui::Begin(consoleTextID);
    {
        std::string clipBoardText;

        for(auto&& message : m_Messages)
        {
            if(!message.find_node("selected").get_value_as<bool>()) 
                continue;

            clipBoardText = clipBoardText.append(fmt::format("[{}][{}][{}] {}", 
                message.find_node("level").get_value(), 
                message.find_node("time").get_value(), 
                message.find_node("logger").get_value(), 
                message.find_node("message").get_value())).append("\n");
        }

        ImGui::TextUnformatted(clipBoardText.c_str());

        // Copy to clipboard
        if(ImGui::IsKeyDown(ImGuiKey::ImGuiMod_Ctrl) && 
            ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_C)) 
            ImGui::SetClipboardText(clipBoardText.c_str());

        ImGui::End();
    }
}

void Console::frame_finish()
{
    Layer::frame_finish();
}

void Console::finish()
{
    Frenchie::Core::Logger::instance()->unregister_sink(
        [](spdlog::sink_ptr _Sink)->bool
        {
            return std::dynamic_pointer_cast<ConsoleSink>(_Sink) != nullptr;
        }
    );
}

bool Console::allows_multiple_instances() const
{
    return false;
}

Console::TReturnType Console::create()
{
    return std::make_unique<Console>();
}