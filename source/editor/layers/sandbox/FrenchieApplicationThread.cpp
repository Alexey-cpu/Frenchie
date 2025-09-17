#include <FrenchieApplicationThread.hpp>

using namespace Frenchie::Application;

Thread::Thread(const std::function<void()> _Worker, const std::string& _Name) : 
    Layer(_Name), 
    m_Worker(_Worker){}

Thread::~Thread(){}

bool Thread::paused() const
{
    return m_Paused;
}

void Thread::pause()
{
    m_Paused = true;
}

void Thread::Thread::resume()
{
    m_Paused = true;
}

void Thread::execute()
{
    while(!m_Finished) 
    {
        // wait untill resumed
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_Wait.wait(lock, [this]()->bool{return m_Paused;});

        // run worker
        if(m_Worker)
            m_Worker();

        // finish task
        m_Finished = true;
    }

    if(m_OnFinished)
        m_OnFinished();
}