#include <FrenchieApplicationEditorConsoleLayer.hpp>

#include <FrenchieApplication.hpp>

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
                    m_Console->m_Messages.push_back(
                        Console::Message(
                            {
                                _Message.time,
                                _Message.level,
                                ConsoleSink::get_color( _Message.level),
                                false,
                                fmt::to_string(_Message.payload)
                            }
                        )
                    );
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
                            return IM_COL32(240, 100, 100, 255);

                        case spdlog::level::level_enum::critical:
                            return IM_COL32(255, 0, 0, 255);
                    }

                    return IM_COL32(255, 255, 255, 255);
                }
            };
        }
    }
}

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
    if(!Layer::awake()) 
        return false;

    Frenchie::Core::Logger::instance()->register_sink<ConsoleSink>(this);

    return true;
}

void Console::frame_update()
{
    ImGui::Begin(get_name().c_str(), &m_Shown);
    {
        // handle events
        if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
        {
            for(auto&& message : m_Messages) 
                message.selected = false;
        }

        // docked windows
        ImGui::Begin("Console");
        {
            // draw clear button
            if(ImGui::Button("Clear"))
                m_Messages.clear();

            // draw message type filter
            for (int i = 0; i < (int)m_MessageTypeFilter.size(); i++)
            {
                ImGui::SameLine();
                ImGui::Checkbox(m_MessageTypeFilter[i].Level.c_str(), &m_MessageTypeFilter[i].Selected);
            }

            // draw maximum message count
            ImGui::SameLine();
            ImGui::SetNextItemWidth(64);
            ImGui::DragInt("MaximumMessageCount", &m_MaximumMessageCount, 1.f, 10, 150);

            // draw message text filter
            ImGui::SameLine();
            auto textFilterLabel = "Text fiter";
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(textFilterLabel).x * 2.f);
            ImGui::InputText(textFilterLabel, m_MessageContentFilter, sizeof(m_MessageContentFilter) / sizeof(m_MessageContentFilter[0]));            

            auto clipRect = ImRect(
                ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos() + ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY()), 
                ImGui::GetWindowContentRegionMax() + ImGui::GetWindowPos() + ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY())
            );

            // draw messages table
            auto id = 0;
            auto messageTextFilter = std::string(m_MessageContentFilter);

            for(auto&& message : m_Messages)
            {
                //check message type filter
                if(!m_MessageTypeFilter[message.level].Selected) 
                    continue;

                //check message text filter
                if(!messageTextFilter.empty() && 
                    !Frenchie::Core::Helpers::String::contains_substring(
                        Helpers::String::to_lower(message.message),
                        Helpers::String::to_lower(messageTextFilter)))
                {
                    continue;
                }

                ImGui::PushID(id++);
                ImGui::PushStyleColor(ImGuiCol_Text, message.color);

                ImGui::GetCursorPos();

                if(ImGui::Selectable(
                    fmt::format(
                        "[{}] {}", 
                        Helpers::String::to_string<std::chrono::system_clock::time_point>(message.time).c_str(), 
                        message.message
                    ).c_str(), 
                    &message.selected, 
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns | 
                    ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowOverlap))
                {
                }

                ImGui::PopStyleColor();
                ImGui::PopID();
            }

            ImGui::End();
        }

        ImGui::End();
    }
}

void Console::finish()
{
    Layer::finish();

    // unregister self from logger
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

bool Console::serialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    // write self
    auto self = _Parent.append_node(STRINGIFY(Console));

    // write message type filter
    auto messageTypeFilter = self.append_node(STRINGIFY(m_MessageTypeFilter));
    for(auto&& item : m_MessageTypeFilter) 
        messageTypeFilter.append_node<bool>(item.Level.c_str(), item.Selected);

    // write message content filter
    self.append_node(
        STRINGIFY(m_MessageContentFilter), 
        std::string(m_MessageContentFilter).c_str(), 
        Frenchie::Core::Serialization::NodeType::OBJECT);

    // write message count
    self.append_node<size_t>(STRINGIFY(m_MaximumMessageCount), m_MaximumMessageCount);

    return true;
}

bool Console::deserialize(const Frenchie::Core::Serialization::Node& _Parent) 
{
    auto self = _Parent.find_node(STRINGIFY(Console));

    if(self.empty()) 
        return false;

    // read message type filter
    auto messageTypeFilter = self.find_node(STRINGIFY(m_MessageTypeFilter));
    for(auto&& item : m_MessageTypeFilter) 
        item.Selected = messageTypeFilter.find_node(item.Level.c_str()).get_value_as<bool>();


    // read message content filter
    // TODO: here we have SEGV !!!
    // std::memcpy(
    //     m_MessageContentFilter,
    //     self.find_node(STRINGIFY(m_MessageContentFilter)).get_value(),
    //     sizeof(m_MessageContentFilter) / sizeof(m_MessageContentFilter[0]));

    // read maximum message count
    m_MaximumMessageCount = 
        self.find_node(STRINGIFY(m_MaximumMessageCount)).get_value_as<int>();

    return true;
}