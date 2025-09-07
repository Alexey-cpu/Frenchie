#pragma once

#include <FrenchieCoreLogger.hpp>
#include <FrenchieCoreSingleton.hpp>
#include <FrenchieCoreNonCopyable.hpp>

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <atomic>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <cassert>

namespace Frenchie
{
    namespace Core
    {
        namespace Thread
        {
            class Pool
            {
            public:
                explicit Pool(size_t = std::thread::hardware_concurrency());
                ~Pool();

                template <typename F, typename... Arguments>
                decltype(auto) enqueue(F&&, Arguments&&...);

                Pool(const Pool&) = delete;
                Pool& operator=(const Pool&) = delete;
                Pool(Pool&&) = delete;
                Pool& operator=(Pool&&) = delete;

            private:

                std::vector<std::thread>          m_Threads;
                std::queue<std::function<void()>> m_Tasks;
                std::mutex                        m_QueueMutex;
                std::condition_variable           m_WaitCondition;
                bool                              m_Stop{false};
            };

            // the constructor just launches some amount of workers
            inline Pool::Pool(size_t _Threads)
            {
                _Threads = std::min<size_t>(std::max<size_t>(_Threads, 2), std::thread::hardware_concurrency());

                for (size_t i = 0; i < _Threads; ++i)
                {                    
                    m_Threads.emplace_back(
                        [this]
                        {
                            for (;;)
                            {
                                std::function<void()> task;
                                {
                                    // lock current thread untill task queue is empty
                                    // ot thread pool is stopped...
                                    std::unique_lock<std::mutex> lock(m_QueueMutex);
                                    
                                    m_WaitCondition.wait(
                                        lock, 
                                        [this] 
                                        { 
                                            return m_Stop || !m_Tasks.empty(); 
                                        }
                                    );
                                    
                                    if (m_Stop && m_Tasks.empty()) 
                                        return;
                                    
                                    // pop current task out of a queue and execute it
                                    task = std::move(m_Tasks.front());
                                    
                                    m_Tasks.pop();
                                }
                                task();
                            }
                        }
                    );
                }
            }

            // the destructor joins all threads
            inline Pool::~Pool()
            {
                {
                    std::unique_lock<std::mutex> lock(m_QueueMutex);
                    m_Stop = true;
                }
                m_WaitCondition.notify_all();
                for (std::thread& worker : m_Threads) { worker.join(); }
            }

            // add new work item to the pool
            template <typename F, typename... Arguments>
            decltype(auto) Pool::enqueue(F&& _Function, Arguments&&... _Arguments)
            {
            #if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
            #if _MSVC_LANG<=201402L
                using return_type = std::result_of_t<F(Args...)>;
            #else
                using return_type = std::invoke_result_t<std::decay_t<F>, std::decay_t<Arguments>...>;
            #endif
            #elif __linux__
            #if __cplusplus <=201402L
                using return_type = std::result_of_t<F(Args...)>;
            #else
                using return_type = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;
            #endif
            #endif

                auto task = std::make_shared<std::packaged_task<return_type()>>(
                    std::bind(std::forward<F>(_Function), std::forward<Args>(_Arguments)...));

                std::future<return_type> res = task->get_future();
                {
                    std::unique_lock<std::mutex> lock(m_QueueMutex);

                    // don't allow enqueueing after stopping the pool
                    if (m_Stop) 
                    { 
                        throw std::runtime_error("enqueue on stopped ThreadPool"); 
                    }

                    m_Tasks.emplace([task](){(*task)();});
                }
                m_WaitCondition.notify_one();
                return res;
            }
        }

        typedef Singleton<Thread::Pool> ThreadPool;

        // class Task
        // {
        // public:
            
        //     Task(std::function<void(Task*)> _Function)
        //     {
        //         try
        //         {
        //             m_Task = TreadPool::instance()->enqueue(
        //                 [this, _Function]()
        //                 {
        //                     if(_Function != nullptr)
        //                         _Function(this);
        //                 }
        //             );
        //         }
        //         catch(const std::exception& e)
        //         {
        //             Frenchie::Core::Logger::instance()->critical(e.what());
        //         }
        //     }
            
        //     ~Task()
        //     {
        //         cancel();
        //     }

        //     bool paused() const
        //     {
        //         return m_Paused;
        //     }

        //     bool finished() const
        //     {
        //         return m_Finished;
        //     }

        //     bool canceled() const
        //     {
        //         return m_Canceled;
        //     }

        //     void pause()
        //     {
        //         m_Paused = true;
        //     }

        //     void cancel()
        //     {
        //         m_Canceled = true;
        //     }

        //     void finish()
        //     {
        //         m_Finished = true;
        //     }
            
        //     void resume()
        //     {
        //         m_Paused = false;
        //     }

        // protected:
        //     std::future<void> m_Task;
        //     bool              m_Paused   = false;
        //     bool              m_Finished = false;
        //     bool              m_Canceled = false;
        // };
    }
}