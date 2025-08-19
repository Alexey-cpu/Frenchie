#include <FrenchieApplicationEditorConsoleLayer.hpp>

#include <mutex>

#include <spdlog/sinks/base_sink.h>
#include <spdlog/details/null_mutex.h>

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
                    if (m_Console == nullptr || m_Console->m_Messages.size() >= m_Console->m_MaximumMessageCount) 
                        return;

                    m_Console->m_Messages.push_back(
                            {
                                _Message.time,
                                _Message.level,
                                fmt::to_string(_Message.payload),
                                fmt::to_string(_Message.logger_name)
                            }
                        );
                }

                virtual void flush_() override{}

            private:

                const Console* m_Console = nullptr;
            };
        }
    }
}

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

// IMGUI
#include <imgui.h>

// Console
Console::Console() : Layer(STRINGIFY(Console))
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
    ImGui::Begin(get_name().c_str(), &m_Opened);

    //if(m_Messages.size() < m_MaximumMessageCount) Frenchie::Core::Logger::instance()->info("INFO !!!");

    // draw clear button
    if(ImGui::Button("Clear")) 
        m_Messages.clear();

    // draw message type filter
    ImGui::SameLine();
    ImGui::SetNextItemWidth(256.f);
    
    std::string messageTypeFilterLabel;
    size_t      counter = 0;

    for (int i = 0; i < (int)m_MessageTypeFilter.size(); i++)
    {
        if(m_MessageTypeFilter[i].Selected) 
            counter++;
    }

    for (int i = 0; i < (int)m_MessageTypeFilter.size(); i++)
    {
        if(m_MessageTypeFilter[i].Selected) 
        {
            messageTypeFilterLabel = 
                messageTypeFilterLabel.append(m_MessageTypeFilter[i].Level).append(" ");
        }

        if(ImGui::CalcTextSize(messageTypeFilterLabel.c_str()).x >= 128) 
        {
            messageTypeFilterLabel = messageTypeFilterLabel.append("...");
            break;
        }
    }

    if(messageTypeFilterLabel.empty()) 
        messageTypeFilterLabel = "none";

    if(counter >= m_MessageTypeFilter.size()) 
        messageTypeFilterLabel = "all";

    if(ImGui::BeginCombo("##", messageTypeFilterLabel.c_str()))
    {
        for (int i = 0; i < (int)m_MessageTypeFilter.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::Checkbox("", &m_MessageTypeFilter[i].Selected);

            ImGui::SameLine();
            if(ImGui::Selectable(
                m_MessageTypeFilter[i].Level.c_str(),
                m_MessageTypeFilter[i].Selected,
                ImGuiSelectableFlags_::ImGuiSelectableFlags_NoAutoClosePopups | 
                ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowOverlap)) 
            {
                m_MessageTypeFilter[i].Selected = !m_MessageTypeFilter[i].Selected;
            }

            ImGui::PopID();
        }

        ImGui::EndCombo();
    }

    ImGui::SameLine();

    auto textFilterLabel = "Text fiter";

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(textFilterLabel).x * 2.f);

    ImGui::InputText(textFilterLabel, 
        m_MessageContentFilter,  
        sizeof(m_MessageContentFilter) / sizeof(m_MessageContentFilter[0]));

    // draw message table
    ImVec2 size  = ImGui::GetContentRegionAvail();
    float  table = 0.7;

    if(ImGui::BeginChild("MessagesTable", ImVec2(0.f, size.y * table), ImGuiChildFlags_::ImGuiChildFlags_Borders))
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
            for(int i = 0; i < (int)m_Messages.size() ; i++)
            {
                // check message type filter 
                if(!m_MessageTypeFilter[m_Messages[i].Level].Selected) 
                    continue;

                // check message text filter
                std::string messageText       = m_Messages[i].Message;
                std::string messageTextFilter = m_MessageContentFilter;

                auto iterator = std::search(
                    messageText.begin(), 
                    messageText.end(), 
                    std::boyer_moore_searcher(messageTextFilter.begin(), messageTextFilter.end()));

                if(!messageTextFilter.empty() && iterator == messageText.end()) 
                    continue;

                ImGui::TableNextRow();
            
                // level
                ImGui::TableSetColumnIndex(0);

                ImGui::PushID(i);     
                
                if(ImGui::Selectable("", 
                    &m_Messages[i].Selected, 
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns | 
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowOverlap)){}

                ImGui::PopID();

                ImGui::SameLine();

                if(m_Messages[i].Level == spdlog::level::level_enum::trace)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128, 255));
                    ImGui::TextUnformatted("trace");
                }
                else if(m_Messages[i].Level == spdlog::level::level_enum::debug)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
                    ImGui::TextUnformatted("debug");
                }
                else if(m_Messages[i].Level == spdlog::level::level_enum::info)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 200, 0, 255));
                    ImGui::TextUnformatted("info");
                }
                else if(m_Messages[i].Level == spdlog::level::level_enum::warn)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(233, 245, 66, 255));
                    ImGui::TextUnformatted("warninig");
                }
                else if(m_Messages[i].Level == spdlog::level::level_enum::err)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(245, 105, 66, 255));
                    ImGui::TextUnformatted("error");
                }
                else if(m_Messages[i].Level == spdlog::level::level_enum::critical)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                    ImGui::TextUnformatted("critical");
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
                    ImGui::TextUnformatted("unknown");
                }

                // date and time
                {
                    ImGui::TableSetColumnIndex(1);
                    std::time_t now = std::chrono::system_clock::to_time_t(m_Messages[i].Time);
                    ImGui::TextUnformatted(std::asctime(std::localtime(&now)));
                }

                // Who
                {
                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextUnformatted(m_Messages[i].LoggerName.c_str());
                }

                // WTF
                {
                    ImGui::TableSetColumnIndex(3);
                    ImGui::TextUnformatted(m_Messages[i].Message.c_str());
                }

                ImGui::PopStyleColor();
            }

            ImGui::EndTable();
        }

        ImGui::EndChild();
    }

    if(ImGui::BeginChild(
        "SelectedMessages", 
        ImVec2(0.f, size.y * (1.f - table)), 
        ImGuiChildFlags_::ImGuiChildFlags_Borders))
    {
        // draw selected messageg preview
        std::string text;
        
        for (int i = 0; i < (int)m_Messages.size(); i++)
        {
            if(m_Messages[i].Selected)
            {
                std::string level;

                if(m_Messages[i].Level == spdlog::level::level_enum::trace) 
                    level = "trace";
                else if(m_Messages[i].Level == spdlog::level::level_enum::debug) 
                    level = "debug";
                else if(m_Messages[i].Level == spdlog::level::level_enum::info) 
                    level = "info";
                else if(m_Messages[i].Level == spdlog::level::level_enum::warn) 
                    level = "warninig";
                else if(m_Messages[i].Level == spdlog::level::level_enum::err) 
                    level = "error";
                else if(m_Messages[i].Level == spdlog::level::level_enum::critical) 
                    level = "critical";
                else 
                    level = "unknown";

                std::time_t now = std::chrono::system_clock::to_time_t(m_Messages[i].Time);
                std::string time = std::string(std::asctime(std::localtime(&now)));
                time.pop_back();
                text += fmt::format("[{}][{}][{}][{}]\n", level, time, m_Messages[i].LoggerName, m_Messages[i].Message);
            }
        }

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::TextUnformatted(text.c_str());

        ImGui::EndChild();
    }

    ImGui::End();
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