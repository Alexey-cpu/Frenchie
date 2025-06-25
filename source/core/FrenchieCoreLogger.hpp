#pragma once

// Custom
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
            class Logger
            {
            public:

                Logger();
                virtual ~Logger();

                template<typename __sink, typename ... __sink_arguments>
                std::shared_ptr<__sink> register_sink(__sink_arguments... _Parameters)
                {   
                    auto sink = std::make_shared<__sink>(_Parameters...);
                    get_logger()->sinks().push_back(sink);
                    return sink;
                }

                void set_level(spdlog::level::level_enum _Level);
                void trace(const std::string _Message);
                void info(const std::string _Message);
                void warn(const std::string _Message);
                void error(const std::string _Message);

            private:

                const std::string m_Name = "Frenchie::Core::Logger";
                mutable std::shared_ptr<spdlog::logger> m_Logger = 
                    std::make_shared<spdlog::logger>(m_Name);

                std::shared_ptr<spdlog::logger>& get_logger() const;
            };
        }

        typedef Singleton<Debug::Logger> Logger;
    }
}