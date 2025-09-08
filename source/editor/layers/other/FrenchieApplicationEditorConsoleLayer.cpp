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
#include <fstream> // Required for ifstream
#include <iostream> // Required for cout
#include <regex>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class TerminalSink : public spdlog::sinks::base_sink<std::mutex>
            {
            public:
                TerminalSink(const Terminal* _Console) : m_Console(_Console){}
                virtual ~TerminalSink(){}

            protected:

                virtual void sink_it_(const spdlog::details::log_msg& _Message) override
                {
                    if(m_Console != nullptr)
                    {
                        auto message = std::string(m_Console->m_Output.get_buffer()).append(fmt::to_string(_Message.payload));

                        m_Console->m_Output.set_buffer(message);
                    }
                }

                virtual void flush_() override{}

            private:

                // info
                const Terminal* m_Console = nullptr;
            };

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
                    m_Console->m_Messages[_Message.level].push_back(
                        Console::Message(
                            {
                                _Message.time,
                                _Message.level,
                                fmt::to_string(_Message.payload)
                            }
                        )
                    );
                }

                virtual void flush_() override{}

            private:

                // info
                const Console* m_Console = nullptr;
            };
        }
    }
}

Terminal::Terminal() : Layer(STRINGIFY(Frenchie::Application::Editor::Terminal)){}
Terminal::~Terminal(){}

bool Terminal::awake()
{
    m_Logger.register_sink<TerminalSink>(this);
    return true;
}

void Terminal::frame_update()
{
    ImGui::Begin(get_name().c_str(), &m_Shown);
    {
        // ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 255));
        // ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 255));
        // ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(0, 0, 0, 255));
        // ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(0, 0, 0, 255));

        m_Output.draw_multiline(
            "Result",
            0,
            0.f,
            ImGui::CalcTextSize(m_Output.get_buffer().c_str()).y + 2.f * ImGui::GetStyle().FramePadding.y);
            

        ImGui::TextUnformatted(Frenchie::Core::String::as_utf8(m_Path.wstring()).c_str());
        ImGui::SameLine();
        
        if(m_Command.draw(
            "Command",
            ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
        {
            // execute command
            {
                // launch command
                std::string command = 
                    fmt::format("cd {} && {} 2>&1", 
                        Frenchie::Core::String::as_utf8(m_Path.wstring()),
                        std::string(m_Command.get_buffer()));

                std::array<char, 128> buffer;
                std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
                
                if (!pipe) 
                {
                    Frenchie::Core::Logger::instance()->critical("Could not open pipe for {}", command);
                    return;
                }

                // retrieve log
                while(fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) 
                    m_Logger.info(Frenchie::Core::String::as_utf8(Frenchie::Core::String::as_wide(buffer.data())));
            }

            // clear command buffer
            m_Command.set_buffer("");
        }

        //ImGui::PopStyleColor(4);

        ImGui::End();
    }
}

void Terminal::finish()
{
    m_Logger.unregister_sink(
        [](spdlog::sink_ptr _Sink)->bool
        {
            return std::dynamic_pointer_cast<TerminalSink>(_Sink) != nullptr;
        }
    );
}

bool Terminal::allows_multiple_instances() const
{
    return true;
}

// Console
Console::Console() : Layer(STRINGIFY(Console))
{
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
        // docked windows
        ImGui::Begin("Console");
        {
            // draw clear button
            if(ImGui::Button("Clear"))
                m_Messages.clear();

            if(ImGui::Button("Debug"))
            {
                // compile program
                std::filesystem::current_path("C:/SDK/Qt_Projects/TestProject");

                const auto out_file = std::filesystem::path("C:/SDK/Qt_Projects/TestProject/logs/messages.txt");
                const auto err_file = std::filesystem::path("C:/SDK/Qt_Projects/TestProject/logs/errors.txt");
                
                std::filesystem::remove_all(out_file);
                std::filesystem::remove_all(err_file);
                
                // 2>&1
                //std::string command = "cmake --preset HabrPresetName"; // Redirect stderr to stdout
                //std::string command = fmt::format("cmake --build --preset debug 2> {} 1> {}", err_file.string(), out_file.string());
                std::string command = fmt::format("cmake --build --preset debug");

                std::array<char, 1024> buffer;
                std::string result;

                std::cout << "Opening reading pipe" << std::endl;

                FILE* pipe = _popen(command.c_str(), "r");
                if (!pipe)
                {
                    std::cerr << "Couldn't start command." << std::endl;
                }
                else
                {
                    while(fgets(buffer.data(), (int)buffer.size(), pipe) != nullptr) 
                        result += buffer.data();
                    
                    // parse compiler log
                    std::smatch matches;
                    std::regex gcc_err_pattern(R"(^(.*?):(\d+):(?:(\d+):)?\s*(error):\s*(.*)$)");
                    std::regex gcc_warn_pattern(R"(^(.*?):(\d+):(?:(\d+):)?\s*(warning):\s*(.*)$)");
                    std::regex gcc_info_pattern(R"(^(.*?):(\d+):(?:(\d+):)?\s*(note):\s*(.*)$)");
                    // std::regex gcc_err_pattern("^(.*?):(\\d+):(?:(\\d+):)?\\s*(error):\\s*(?:\\s*\\[([^\\]]+)\\])?\\s*(.*)$");
                    // std::regex gcc_warn_pattern("^(.*?):(\\d+):(?:(\\d+):)?\\s*(warning):\\s*(?:\\s*\\[([^\\]]+)\\])?\\s*(.*)$");
                    // std::regex gcc_info_pattern("^(.*?):(\\d+):(?:(\\d+):)?\\s*(note):\\s*(?:\\s*\\[([^\\]]+)\\])?\\s*(.*)$");
                    std::regex failed_pattern(R"(^FAILED:\s*(.*)$)", std::regex_constants::icase);

                    if (std::regex_search(result, matches, failed_pattern))
                        Frenchie::Core::Logger::instance()->critical(matches[0].str());

                    if (std::regex_search(result, matches, gcc_err_pattern))
                        Frenchie::Core::Logger::instance()->error(matches[0].str());
                    
                    if(std::regex_search(result, matches, gcc_warn_pattern))
                        Frenchie::Core::Logger::instance()->warn(matches[0].str());

                    if(std::regex_search(result, matches, gcc_info_pattern))
                        Frenchie::Core::Logger::instance()->debug(matches[0].str());

                    Frenchie::Core::Logger::instance()->trace(result);

                    auto returnCode = _pclose(pipe);
                }
            }

            // draw maximum message count
            ImGui::SameLine();
            ImGui::SetNextItemWidth(64);
            ImGui::DragInt("MaximumMessageCount", &m_MaximumMessageCount, 1.f, 10, 150);

            // draw message text filter
            //ImGui::SameLine();
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
            
            if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
            {
                for (int level = 0; level < spdlog::level::level_enum::n_levels - 1; level++)
                {
                    std::string  group;

                    for(auto&& message : m_Messages[(spdlog::level::level_enum)level])
                    {
                        // check message type
                        if(message.level != level) 
                            continue;

                        // check message text filter
                        if(!messageTextFilter.empty() && 
                            !Frenchie::Core::String::contains_substring(
                                Core::String::to_lower(message.message),
                                Core::String::to_lower(messageTextFilter)))
                        {
                            continue;
                        }

                        group.append(message.message).append("\n");
                    }

                    auto appTextColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
                    ImGui::PushStyleColor(ImGuiCol_Tab, Console::get_color((spdlog::level::level_enum)level));
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));

                    auto tabID = m_Messages[(spdlog::level::level_enum)level].size() > 0 ?
                        fmt::format("{} +{} !!!", Console::get_group_name((spdlog::level::level_enum)level), m_Messages[(spdlog::level::level_enum)level].size()) : 
                            Console::get_group_name((spdlog::level::level_enum)level);

                    if(ImGui::BeginTabItem(tabID.c_str()))
                    {
                        if(!group.empty())
                        {
                            ImGui::PushStyleColor(ImGuiCol_Text, Console::get_color((spdlog::level::level_enum)level));
                            ImGui::PushID(id++);
                            ImGui::InputTextMultiline(
                                    "##", 
                                    &group[0], 
                                    group.size() + 1, 
                                    ImGui::GetContentRegionAvail(), 
                                    ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly
                                );

                            ImGui::PopID();
                            ImGui::PopStyleColor();
                        }
                        else
                        {
                            ImGui::PushStyleColor(ImGuiCol_Text, appTextColor);
                            ImGui::TextUnformatted("Nothing to show...");
                            ImGui::PopStyleColor();
                        }

                        ImGui::EndTabItem();
                    }

                    ImGui::PopStyleColor();
                    ImGui::PopStyleColor();
                }

                ImGui::EndTabBar();
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

unsigned int Console::get_color(spdlog::level::level_enum _Level)
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

std::string Console::get_group_name(spdlog::level::level_enum _Level)
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

bool Console::serialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    // write self
    auto self = _Parent.append_node(STRINGIFY(Console));

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

    // read message content filter
    if(self.find_node(STRINGIFY(m_MessageContentFilter)).is_valid())
    {
        std::memcpy(
            m_MessageContentFilter,
            self.find_node(STRINGIFY(m_MessageContentFilter)).get_value(),
            std::min(sizeof(m_MessageContentFilter) / sizeof(m_MessageContentFilter[0]), std::strlen(self.find_node(STRINGIFY(m_MessageContentFilter)).get_value())));
    }

    // read maximum message count
    m_MaximumMessageCount = 
        self.find_node(STRINGIFY(m_MaximumMessageCount)).get_value_as<int>();

    return true;
}