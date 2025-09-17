#pragma once

// Core
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreLogger.hpp>

// Application
#include <FrenchieApplication.hpp>

// STL
#include <regex>

namespace Frenchie
{
    namespace Editor
    {
        // TerminalLogger
        class TerminalLogger : public Frenchie::Core::Debug::Logger
        {
        public:
            TerminalLogger() : TerminalLogger::Logger(STRINGIFY(Frenchie::Core::Logger)){}
            virtual ~TerminalLogger(){}

            static TerminalLogger* instance()
            {
                return Frenchie::Core::Singleton<TerminalLogger>::instance();
            }
        };

        // Terminal
        class Terminal : public Frenchie::Application::Layer
        {
        public:
            Terminal();
            virtual ~Terminal();

            // Layer
            virtual bool awake() override;
            virtual bool allows_multiple_instances() const override;

            template<typename ... Arguments>
            void execute(const std::string _Command, Arguments ... _Args)
            {
                // execute command
                #ifdef WIN32
                std::string command = fmt::format("{} {} 2>&1", _Command, _Args...);
                std::array<char, 128> buffer;
                std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
                #else
                std::string command = fmt::format("{} {} 2>&1", _Command, _Args...);
                std::array<char, 128> buffer;
                std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
                #endif

                if (!pipe) 
                {
                    Frenchie::Core::Logger::instance()->critical("Could not open pipe for {}", command);
                    return;
                }

                std::string result;

                while(fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) 
                {
                    // reuslt log of Frenchie core logger
                    result += buffer.data();

                    // trace log of terminal plain text logger
                    TerminalLogger::instance()->trace(
                        Frenchie::Core::String::as_utf8(
                            Frenchie::Core::String::as_wide(buffer.data())));
                }

                // try parse terminal output using std::regex
                {
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
                }
            }

            // API
            static Frenchie::Core::Reference<Terminal> instance();
        };
    }
}