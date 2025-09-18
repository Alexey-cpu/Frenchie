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
        namespace Threads
        {
            class Pool final : public NonCopyable
            {
            public:
                
                Pool(const size_t& = std::thread::hardware_concurrency());
                ~Pool();

                void enqueue(const std::function<void()>&);

            protected:
                std::vector<std::thread>          m_Threads;
                std::queue<std::function<void()>> m_Tasks;
                std::mutex                        m_Mutex;
                std::condition_variable           m_Wait;
                bool                              m_Stop{false};
            };
        }

        typedef Singleton<Threads::Pool> ThreadPool;
    }
}