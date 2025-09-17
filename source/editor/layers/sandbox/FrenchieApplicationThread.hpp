#pragma once

#include <FrenchieApplication.hpp>

namespace Frenchie
{
    namespace Application
    {
        class Thread
        {
        public:
            Thread(const std::function<void()> _Worker, const std::function<void()> _OnFinished, const std::string& _Name = STRINGIFY(Thread));
            virtual ~Thread();

            // API
            bool paused() const;

            void pause();
            void resume();
            void execute();

        private:
            std::function<void()> m_Worker    {nullptr};
            std::function<void()> m_OnFinished{nullptr};

            std::atomic<bool>     m_Paused  {false};
            std::atomic<bool>     m_Finished{false};

            std::condition_variable m_Wait;
            std::mutex              m_Mutex;
        };
    }
}