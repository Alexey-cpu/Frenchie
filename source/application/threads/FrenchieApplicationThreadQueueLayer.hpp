#pragma once

// Core
#include <FrenchieCoreStringConvert.hpp>

// Application
#include <FrenchieApplication.hpp>

// STL
#include <functional>
#include <list>

namespace Frenchie
{
    namespace Application
    {
        class ThreadComponent
        {
        public:
            ThreadComponent(){}
            virtual ~ThreadComponent(){}
        };

        class ThreadStatusComponent : public ThreadComponent
        {
        public:
            ThreadStatusComponent(){}
            virtual ~ThreadStatusComponent(){}

            // getters
            std::string get_status() const
            {
                return m_Status.m_Buffer;
            }

            // setters
            void push_message(const std::string& _Message)
            {
                m_Status.push(_Message);
            }

        protected:

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
            } mutable m_Status;
        };

        class ThreadProgressComponent : public ThreadComponent
        {
        public:
            ThreadProgressComponent(){}
            virtual ~ThreadProgressComponent(){}

            // getters
            float get_progress()
            {
                return m_Progress;
            }

            // setters
            void set_progress(const float& _Value)
            {
                m_Progress = _Value;
            }

        protected:
            float m_Progress = 0.f;
        };

        class Thread
        {
        public:
            Thread(std::function<void(const Thread*)> _Worker, const std::string& _Name = std::string());
            virtual ~Thread();

            std::string get_name() const;

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

            bool requested_stop() const
            {
                while(paused() && !stopped()); // wait while paused

                if(stopped()) 
                    return true;

                return false;
            }

            template<typename Type, typename ... Arguments>
            Frenchie::Core::Reference<Type> attach_component(Arguments ... _Args)
            {
                return std::make_shared<Type>(_Args...);
            }

            template<typename Type>
            Frenchie::Core::Reference<Type> find_component() const
            {
                for(auto&& component : m_Components)
                {
                    if(std::dynamic_pointer_cast<Type>(component) != nullptr)
                        return component;
                }

                return nullptr;
            }

        protected:

            friend class ThreadQueue;

            std::list<std::shared_ptr<ThreadComponent>> m_Components;

            // info
            std::string                 m_Name     = STRINGIFY(Thread);
            mutable bool                m_Paused   = false;
            mutable bool                m_Finished = false;
            mutable bool                m_Stopped  = false;
            mutable bool                m_Failed   = false;

            std::function<void(const Thread*)> m_Worker;
            std::function<void(const Thread*)> m_OnFinished;
            std::function<void(const Thread*)> m_OnStopped; 
            std::function<void(const Thread*)> m_OnFailed;

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

            Frenchie::Core::Reference<Thread> push(std::function<void(const Thread*)> _Worker, const std::string& _Name = std::string());
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