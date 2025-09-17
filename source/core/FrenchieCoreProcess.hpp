#pragma once

// Core
#include <FrenchieCoreSingleton.hpp>
#include <FrenchieCoreThreadPool.hpp>

// STL
#include <queue>
#include <string>
#include <functional>

namespace Frenchie
{
    namespace Core
    {
        class Process
        {
        public:
            Process(const std::string& _Command, const std::string& _Arguments = std::string());
            ~Process();

            // getters
            std::string get_status() const;

            // API
            void stop();
            void pause();
            void resume();

            bool alive() const;
            bool exited() const;
            bool paused() const;
            bool stopped() const;

        private:

            class Status final
            {
            public:
                Status()
                {
                    m_Buffer.resize(4096);
                }

                ~Status(){}

                void push(const std::string& _Value)
                {
                    for (size_t i = 0; i < _Value.size(); i++)
                        m_Buffer[(m_CurrentPosition++) % m_Buffer.size()] = _Value[i];
                }

                std::string m_Buffer          = std::string();
                size_t      m_CurrentPosition = 0;
            } m_Status;

            mutable int                          m_PID    = -1;
            mutable std::mutex                   m_Mutex;
            mutable Frenchie::Core::Thread::Pool m_Pool{1};
        };
    }
}