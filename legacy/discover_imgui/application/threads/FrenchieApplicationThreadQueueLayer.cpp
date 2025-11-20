#include <FrenchieApplicationThreadQueueLayer.hpp>

// Core
#include <FrenchieCoreThreadPool.hpp>

// Application
#include <FrenchieApplication.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;

// Thread
Thread::Thread(std::function<void(const Thread*)> _Worker, const std::string& _Name) : 
    m_Worker(_Worker),
    m_Name(_Name){}

Thread::~Thread()
{
    stop(); // cancel on destroy
}

std::string Thread::get_name() const
{
    return m_Name;
}

float Thread::get_progress() const
{
    return m_Progress;
}

std::string Thread::get_status() const
{
    return m_Status.m_Buffer;
}

void Thread::push_message(const std::string& _Message) const
{
    m_Status.push(_Message);
}

void Thread::set_progress(const float& _Value) const
{
    m_Progress = _Value;
}

void Thread::stop() const
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Stopped = true;
}

void Thread::pause() const
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Paused = true;
}

void Thread::resume() const
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

bool Thread::launch()
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
            catch(...)
            {
                // fail if something went wrong...
                m_Failed = true;
            }
            
        }
    );
    return true;
}

bool Thread::requested_stop() const
{
    while(paused() && !stopped()); // wait while paused

    if(stopped()) 
        return true;

    return false;
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

ThreadQueue::ThreadQueue(){}
ThreadQueue::~ThreadQueue(){}

void ThreadQueue::frame_start()
{
    // launch
    while(!m_Queue.empty())
    {
        auto process = m_Queue.front();

        if(process != nullptr && process->launch())
            m_Executed.push_back(process);
        m_Queue.pop();
    }

    // finish
    for(auto it = m_Executed.begin(); it != m_Executed.end(); it++)
    {
        if((*it)->finished() || (*it)->stopped() || (*it)->failed())
        {
            // call finish method only if thread has not been stopped
            // and has not failed it's operation
            if(!(*it)->stopped() && !(*it)->failed())
                (*it)->finish();

            auto rm = it;
            it++;
            m_Executed.erase(rm);

            if(it == m_Executed.end())
                break;
        }
    }
}

void ThreadQueue::finish()
{
    // cancel all on finish
    for(auto it = m_Executed.begin(); it != m_Executed.end(); it++)
        (*it)->stop();
}

bool ThreadQueue::allows_multiple_instances() const
{
    return false;
}

Frenchie::Core::Reference<Thread> ThreadQueue::push(
    std::function<void(const Thread*)> _Worker,
    const std::string&                 _Name)
{
    m_Queue.push(std::make_shared<Thread>(_Worker, _Name));
    return m_Queue.back();
}

bool ThreadQueue::contains(const std::string& _ThreadName) const
{
    for(auto it = begin(); it != end(); ++it)
    {
        if((*it)->get_name() == _ThreadName)
            return true;
    }

    return false;
}

std::list<std::shared_ptr<Thread>>::const_iterator ThreadQueue::begin() const
{
    return m_Executed.begin();
}

std::list<std::shared_ptr<Thread>>::const_iterator ThreadQueue::end() const
{
    return m_Executed.end();
}

bool ThreadQueue::empty() const
{
    return m_Queue.empty() && m_Executed.empty();
}