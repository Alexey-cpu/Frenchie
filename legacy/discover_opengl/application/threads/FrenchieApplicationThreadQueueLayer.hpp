#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>

// Core
#include <FrenchieCoreReference.hpp>

// STL
#include <functional>
#include <queue>

namespace Frenchie
{
    namespace Application
    {
        class Thread
        {
        public:
            Thread(std::function<void(const Thread*)> _Worker, const std::string& _Name = std::string());
            virtual ~Thread();

            // getters
            std::string get_name() const;
            float get_progress() const;
            std::string get_status() const;

            // setters
            void push_message(const std::string&) const;
            void set_progress(const float&) const;

            // API
            void stop() const;
            void pause() const;
            void resume() const;

            bool stopped() const;
            bool paused() const;
            bool finished() const;
            bool failed()   const;

            void on_finished(const std::function<void(const Thread*)>& _Callback);
            void on_stopped(const std::function<void(const Thread*)>& _Callback);
            void on_failed(const std::function<void(const Thread*)>& _Callback);

            bool launch();
            bool requested_stop() const;

        protected:

            friend class ThreadQueue;

            // info
            std::string  m_Name     = STRINGIFY(Thread);
            mutable bool m_Paused   = false;
            mutable bool m_Finished = false;
            mutable bool m_Stopped  = false;
            mutable bool m_Failed   = false;

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

                std::string m_Buffer         {std::string()};
                size_t      m_CurrentPosition{0};
            };

            mutable Status                     m_Status    {Status()};
            mutable float                      m_Progress  {0.f};
            std::function<void(const Thread*)> m_Worker    {nullptr};
            std::function<void(const Thread*)> m_OnFinished{nullptr};
            std::function<void(const Thread*)> m_OnStopped {nullptr}; 
            std::function<void(const Thread*)> m_OnFailed  {nullptr};

            // synchronization
            mutable std::mutex m_Mutex;

            // service methods
            void finish();
        };

        class ThreadQueue : public Layer
        {
        public:
            ThreadQueue();
            virtual ~ThreadQueue();

            virtual void frame_start();
            virtual void finish();
            virtual bool allows_multiple_instances() const;

            virtual Frenchie::Core::Reference<Thread> push(
                std::function<void(const Thread*)> _Worker,
                const std::string&                 _Name = std::string());
            
            bool contains(const std::string&) const;
            std::list<std::shared_ptr<Thread>>::const_iterator begin() const;
            std::list<std::shared_ptr<Thread>>::const_iterator end() const;
            bool empty() const;

            typedef std::list<std::shared_ptr<Thread>>::const_iterator const_iterator;

        protected:

            std::queue<std::shared_ptr<Thread>> m_Queue = 
                std::queue<std::shared_ptr<Thread>>();

            std::list<std::shared_ptr<Thread>> m_Executed =    
                std::list<std::shared_ptr<Thread>>();
        };
    }
}