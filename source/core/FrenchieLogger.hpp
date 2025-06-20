#pragma once

// STL
#include <memory>

// SPDLOG
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h> // or stdout_sinks.h for no color
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

namespace Frenchie
{
    class Logger
    {
    public:

        static Logger* instance()
        {
            static Logger s;
            return &s;
        }

        Logger(const Logger &) = delete;
        Logger & operator = (const Logger &) = delete;

        template<typename __sink, typename ... __sink_arguments>
        std::shared_ptr<__sink> register_sink(__sink_arguments... _Parameters)
        {   
            auto sink = std::make_shared<__sink>(_Parameters...);
            get_logger()->sinks().push_back(sink);
            return sink;
        }

        void set_level(spdlog::level::level_enum _Level)
        {
            get_logger()->set_level(_Level);
        }

        void trace(const std::string _Message)
        {
            get_logger()->trace(_Message);
        }

        void info(const std::string _Message)
        {
            get_logger()->info(_Message);
        }

        void warn(const std::string _Message)
        {
            get_logger()->warn(_Message);
        }

        void error(const std::string _Message)
        {
            get_logger()->error(_Message);
        }

    private:

        Logger()
        {
            spdlog::register_logger(m_Logger);
        }
        
        ~Logger(){}

        const std::string m_Name = "Bulldog";
        mutable std::shared_ptr<spdlog::logger> m_Logger = 
            std::make_shared<spdlog::logger>(m_Name);

        std::shared_ptr<spdlog::logger>& get_logger() const
        {
            return m_Logger;
        }
    };
}