#pragma once

// Custom
#include <FrenchieCoreNonCopyable.hpp>
#include <FrenchieCoreSingleton.hpp>

// STL
#include <memory>

// SPDLOG
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

// FMT
#include <fmt/format.h>

namespace Frenchie
{
    namespace Core
    {
        namespace Debug
        {
            class Logger : public NonCopyable
            {
            public:

                Logger(const std::string& = "Frenchie::Core::Debug::Logger");
                virtual ~Logger();

                template<typename __sink, typename ... __sink_arguments>
                std::shared_ptr<__sink> register_sink(__sink_arguments... _Parameters)
                {   
                    auto sink = std::make_shared<__sink>(_Parameters...);
                    get_logger()->sinks().push_back(sink);
                    return sink;
                }

                void unregister_sink(std::function<bool(spdlog::sink_ptr)> _Predidate)
                {
                    auto& vec = get_logger()->sinks();

                    vec.erase(std::remove_if(vec.begin(), vec.end(), _Predidate), vec.end());
                }

                void set_level(spdlog::level::level_enum _Level);

                template<typename ... Arguments>
                void trace(const std::string _Message, Arguments ... _Args)
                {
                    get_logger()->trace(fmt::format(_Message, _Args ...));
                }

                template<typename ... Arguments>
                void debug(const std::string _Message, Arguments ... _Args)
                {
                    get_logger()->debug(fmt::format(_Message, _Args ...));
                }

                template<typename ... Arguments>
                void info(const std::string _Message, Arguments ... _Args)
                {
                    get_logger()->info(fmt::format(_Message, _Args ...));
                }

                template<typename ... Arguments>
                void warn(const std::string _Message, Arguments ... _Args)
                {
                    get_logger()->warn(fmt::format(_Message, _Args ...));
                }

                template<typename ... Arguments>
                void error(const std::string _Message, Arguments ... _Args)
                {
                    get_logger()->error(fmt::format(_Message, _Args ...));
                }

                template<typename ... Arguments>
                void critical(const std::string _Message, Arguments ... _Args)
                {
                    get_logger()->critical(fmt::format(_Message, _Args ...));
                }

            private:

                const std::string                       m_Name   = "Frenchie::Core::Debug::Logger";
                mutable std::shared_ptr<spdlog::logger> m_Logger = nullptr;

                std::shared_ptr<spdlog::logger>& get_logger() const;
            };
        }

        class Logger : public Debug::Logger
        {
        public:
            Logger() : Debug::Logger("Frenchie::Core::Logger"){}
            virtual ~Logger(){}

            static Logger* instance()
            {
                return Singleton<Logger>::instance();
            }
        };
    }
}