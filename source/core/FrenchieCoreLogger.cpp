#include <FrenchieCoreLogger.hpp>

using namespace Frenchie::Core::Debug;

Logger::Logger()
{
    spdlog::register_logger(m_Logger);
}

Logger::~Logger(){}

void Logger::set_level(spdlog::level::level_enum _Level)
{
    get_logger()->set_level(_Level);
}

void Logger::trace(const std::string _Message)
{
    get_logger()->trace(_Message);
}

void Logger::info(const std::string _Message)
{
    get_logger()->info(_Message);
}

void Logger::warn(const std::string _Message)
{
    get_logger()->warn(_Message);
}

void Logger::error(const std::string _Message)
{
    get_logger()->error(_Message);
}

std::shared_ptr<spdlog::logger>& Logger::get_logger() const
{
    return m_Logger;
}