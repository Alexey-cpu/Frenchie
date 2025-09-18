#pragma once

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
            Frenchie::Core::UUID4 get_uuid() const;

            // API
            void stop();
            void pause();
            void resume();

            bool stopped() const;
            bool paused() const;
            bool finished() const;
            bool failed()   const;

            void on_finished(const std::function<void(const Thread*)>& _Callback);
            void on_stopped(const std::function<void(const Thread*)>& _Callback);
            void on_failed(const std::function<void(const Thread*)>& _Callback);

            bool awake();

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
            std::string           m_Name     = STRINGIFY(Thread);
            Frenchie::Core::UUID4 m_UUID;
            bool                  m_Paused   = false;
            bool                  m_Finished = false;
            bool                  m_Stopped  = false;
            bool                  m_Failed   = false;

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
            ThreadQueue(){}
            virtual ~ThreadQueue(){}

            virtual void frame_start()
            {
                // launch
                while(!m_Queue.empty())
                {
                    auto process = m_Queue.front();

                    if(process != nullptr && process->awake())
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
            
            virtual void finish()
            {
                // cancel all on finish
                for(auto it = m_Executed.begin(); it != m_Executed.end(); it++)
                    (*it)->stop();
            }

            virtual bool allows_multiple_instances() const
            {
                return true;
            }

            Frenchie::Core::Reference<Thread> push(std::function<void(const Thread*)> _Worker, const std::string& _Name = std::string())
            {
                m_Queue.push(std::make_shared<Thread>(_Worker, _Name));
                return m_Queue.back();
            }

            std::list<std::shared_ptr<Thread>>::const_iterator begin() const
            {
                return m_Executed.begin();
            }

            std::list<std::shared_ptr<Thread>>::const_iterator end() const
            {
                return m_Executed.end();
            }

        protected:

            std::queue<std::shared_ptr<Thread>> m_Queue = 
                std::queue<std::shared_ptr<Thread>>();

            std::list<std::shared_ptr<Thread>> m_Executed =    
                std::list<std::shared_ptr<Thread>>();
        };
    }
}