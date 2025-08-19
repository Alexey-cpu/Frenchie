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
Console::Console() : 
    Layer(STRINGIFY(Console)){}

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

    if(ImGui::Button("Add trace"))
        Frenchie::Core::Logger::instance()->trace("Hello motherfucker !!!");

    if(ImGui::Button("Add info"))
        Frenchie::Core::Logger::instance()->info("Hello motherfucker !!!");

    if(ImGui::Button("Add warn"))
        Frenchie::Core::Logger::instance()->warn("Hello motherfucker !!!");

    if(ImGui::Button("Add error"))
        Frenchie::Core::Logger::instance()->error("Hello motherfucker !!!");

    bool selected = true;

    if (ImGui::BeginCombo("##", "Message filter", ImGuiComboFlags_::ImGuiComboFlags_HeightLarge))
    {
        for (int i = 0; i < (int)m_Filters.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::Checkbox("", &m_Filters[i].Selected);

            ImGui::SameLine();
            if(ImGui::Selectable(
                m_Filters[i].Level.c_str(),
                m_Filters[i].Selected,
                ImGuiSelectableFlags_::ImGuiSelectableFlags_NoAutoClosePopups)) 
                    m_Filters[i].Selected = !m_Filters[i].Selected;

            ImGui::PopID();
        }

        ImGui::EndCombo();
    }

    if(ImGui::BeginTable(
        "Messages", 
        4, 
        ImGuiTableFlags_::ImGuiTableFlags_Borders   | 
        ImGuiTableFlags_::ImGuiTableFlags_Resizable | 
        ImGuiTableFlags_::ImGuiTableFlags_Reorderable))
    {
        ImGui::TableSetupColumn("time");
        ImGui::TableSetupColumn("level");
        ImGui::TableSetupColumn("logger");
        ImGui::TableSetupColumn("message");
        ImGui::TableHeadersRow();

        // draw a content of spdlog buffer here
        for(auto&& message : m_Messages)
        {
            ImGui::TableNextRow();

            // date and time
            {
                ImGui::TableSetColumnIndex(0);

                std::time_t now = std::chrono::system_clock::to_time_t(message.Time);
                ImGui::TextUnformatted(std::asctime(std::localtime(&now)));
            }

            // level
            {
                ImGui::TableSetColumnIndex(1);

                switch (message.Level)
                {
                case spdlog::level::level_enum::trace:
                    ImGui::TextUnformatted("Trace");
                    break;

                case spdlog::level::level_enum::debug:
                    ImGui::TextUnformatted("Debug");
                    break;

                case spdlog::level::level_enum::info:
                    ImGui::TextUnformatted("Info");
                    break;

                case spdlog::level::level_enum::warn:
                    ImGui::TextUnformatted("Warn");
                    break;

                case spdlog::level::level_enum::err:
                    ImGui::TextUnformatted("Error");
                    break;

                case spdlog::level::level_enum::critical:
                    ImGui::TextUnformatted("Critical");
                    break;
                
                default:
                    ImGui::TextUnformatted("Unknown");
                    break;
                }
            }

            // Who
            {
                ImGui::TableSetColumnIndex(2);

                ImGui::TextUnformatted(message.LoggerName.c_str());
            }

            // WTF
            {
                ImGui::TableSetColumnIndex(3);

                ImGui::TextUnformatted(message.Message.c_str());
            }
        }

        ImGui::EndTable();
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