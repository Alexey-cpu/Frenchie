#include <FrenchieApplicationAsynchronousProcessesLayer.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;

Process::Process(const std::string& _Name) : 
    m_Name(_Name), 
    m_UUID(Singleton<UUID4Generator>::instance()->guid()){}

Process::~Process()
{
    cancel(); // cancel on destroy
}

void Process::pause()
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Paused = true;
}

void Process::resume()
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Paused = false;
}

void Process::cancel()
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Canceled = true;
}

bool Process::paused() const
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_Paused;
}

bool Process::canceled() const
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_Canceled;
}

bool Process::finished() const
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_Finished;
}

bool Process::failed() const
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_Failed;
}

void Process::on_finished(const std::function<void()>& _Callback)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_OnFinished = _Callback;
}

void Process::on_canceled(const std::function<void()>& _Callback)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_OnCanceled = _Callback;
}

void Process::on_failed(const std::function<void()>& _Callback)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_OnFailed = _Callback;
}

// void Process::frame_finish()
// {
//     // remove process out-of application queue
//     // when it finished...
//     if(finished() || canceled() || failed()) 
//         close();
// }

void Process::finish()
{
    Frenchie::Core::Logger::instance()->warn("Process::finish()");

    if(finished())
        invokeOnFinished();
    else if(canceled()) 
        invokeOnCanceled();
    else if(failed())
        invokeOnFailed();
}

void Process::invokeOnFinished()
{
    Frenchie::Core::Logger::instance()->warn("Process::invokeOnFinished()");

    std::unique_lock<std::mutex> lock(m_Mutex);

    if(m_OnFinished != nullptr)
        m_OnFinished();
}

void Process::invokeOnCanceled()
{
    Frenchie::Core::Logger::instance()->warn("Process::invokeOnCanceled()");

    std::unique_lock<std::mutex> lock(m_Mutex);

    if(m_OnCanceled != nullptr)
        m_OnCanceled();
}

void Process::invokeOnFailed()
{
    Frenchie::Core::Logger::instance()->warn("Process::invokeOnFailed()");

    std::unique_lock<std::mutex> lock(m_Mutex);

    if(m_OnFailed != nullptr)
        m_OnFailed();
}