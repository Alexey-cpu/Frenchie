#include <FrenchieCoreLogger.hpp>

using namespace Frenchie::Core::Debug;

Logger::Logger(const std::string& _Name) : 
    m_Name(_Name), 
    m_Logger(std::make_shared<spdlog::logger>(m_Name))
{
    spdlog::register_logger(m_Logger);
}

Logger::~Logger(){}

void Logger::set_level(spdlog::level::level_enum _Level)
{
    get_logger()->set_level(_Level);
}

std::shared_ptr<spdlog::logger>& Logger::get_logger() const
{
    return m_Logger;
}