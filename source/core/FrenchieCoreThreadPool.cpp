#include <FrenchieCoreThreadPool.hpp>

using namespace Frenchie::Core::Thread;

Pool::Pool(const size_t& _Threads)
{
    for (size_t thread = 0; thread < 
        std::min<size_t>(_Threads, std::thread::hardware_concurrency()); thread++)
    {
        m_Threads.emplace_back(
            [this]()
            {
                while(true)
                {
                    std::function<void()> task;
                    
                    // here we should place brackets as 'std::unique_lock<std::mutex>'
                    // unlocks only when it goes out-of scope...
                    {
                        std::unique_lock<std::mutex> lock(m_Mutex);

                        m_Wait.wait(
                            lock, 
                            [this]()->bool
                            {
                                return m_Stop || !m_Tasks.empty();
                            }
                        );
                        
                        if(m_Stop || m_Tasks.empty()) return;

                        task = std::move(m_Tasks.front());
                        
                        m_Tasks.pop();
                    }

                    task();
                }    
            }
        );
    }
}

Pool::~Pool()
{
    // here we should place brackets as 'std::unique_lock<std::mutex>'
    // unlocks only when it goes out-of scope...
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_Stop = true;
    }

    m_Wait.notify_all();

    for(auto&& thread : m_Threads)
        thread.join();
}

void Pool::enqueue(const std::function<void()>& _Function)
{
    // here we should place brackets as 'std::unique_lock<std::mutex>'
    // unlocks only when it goes out-of scope...
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        
        if(!m_Stop)
            m_Tasks.emplace(std::move(_Function));
    }
    
    // notify a single thread to start executing task
    m_Wait.notify_one();
}