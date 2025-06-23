#include <FrenchieCoreLogger.hpp>

using namespace Frenchie::Core;

DebugLog::DebugLog()
{
    spdlog::register_logger(m_Logger);
}

DebugLog::~DebugLog(){}

void DebugLog::set_level(spdlog::level::level_enum _Level)
{
    get_logger()->set_level(_Level);
}

void DebugLog::trace(const std::string _Message)
{
    get_logger()->trace(_Message);
}

void DebugLog::info(const std::string _Message)
{
    get_logger()->info(_Message);
}

void DebugLog::warn(const std::string _Message)
{
    get_logger()->warn(_Message);
}

void DebugLog::error(const std::string _Message)
{
    get_logger()->error(_Message);
}

std::shared_ptr<spdlog::logger>& DebugLog::get_logger() const
{
    return m_Logger;
}