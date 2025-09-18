#include <FrenchieApplicationThreadQueueLayer.hpp>

#include <FrenchieCoreThreadPool.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;

Thread::Thread(std::function<void(const Thread*)> _Worker, const std::string& _Name) : 
    m_Worker(_Worker),
    m_Name(_Name), 
    m_UUID(Singleton<UUID4Generator>::instance()->guid()){}

Thread::~Thread()
{
    stop(); // cancel on destroy
}

std::string Thread::get_name() const
{
    return m_Name;
}

Frenchie::Core::UUID4 Thread::get_uuid() const
{
    return m_UUID;
}

void Thread::stop()
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Stopped = true;
}

void Thread::pause()
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Paused = true;
}

void Thread::resume()
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Paused = false;
}

bool Thread::stopped() const
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_Stopped;
}

bool Thread::paused() const
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_Paused;
}

bool Thread::finished() const
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_Finished;
}

bool Thread::failed() const
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_Failed;
}

void Thread::on_finished(const std::function<void(const Thread*)>& _Callback)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_OnFinished = _Callback;
}

void Thread::on_stopped(const std::function<void(const Thread*)>& _Callback)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_OnStopped = _Callback;
}

void Thread::on_failed(const std::function<void(const Thread*)>& _Callback)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_OnFailed = _Callback;
}

bool Thread::awake()
{
    Frenchie::Core::ThreadPool::instance()->enqueue(
        [this]()
        {
            try
            {
                // execute worker function
                if(m_Worker) 
                    m_Worker(this);

                // finish thread
                m_Finished = true;
            }
            catch(const std::exception& e)
            {
                // fail if something went wrong...
                m_Failed = true;
            }
            
        }
    );
    return true;
}

void Thread::finish()
{
    if(finished())
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        if(m_OnFinished != nullptr)
            m_OnFinished(this);
    }
    else if(stopped()) 
    {
        std::unique_lock<std::mutex> lock(m_Mutex);

        if(m_OnStopped != nullptr)
            m_OnStopped(this);
    }
    else if(failed())
    {
        std::unique_lock<std::mutex> lock(m_Mutex);

        if(m_OnFailed != nullptr)
            m_OnFailed(this);
    }
}