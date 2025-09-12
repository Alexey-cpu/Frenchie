#pragma once

#include <FrenchieApplication.hpp>

// STL
#include <functional>

namespace Frenchie
{
    namespace Application
    {
        class Process
        {
        public:
            Process(const std::string& _Name = std::string());
            virtual ~Process();

            std::string get_name() const
            {
                return m_Name;
            }

            Frenchie::Core::UUID4 get_uuid() const
            {
                return m_UUID;
            }

            // API
            void pause();
            void resume();
            void cancel();

            bool paused()   const;
            bool canceled() const;
            bool finished() const;
            bool failed()   const;

            void on_finished(const std::function<void()>& _Callback);
            void on_canceled(const std::function<void()>& _Callback);
            void on_failed(const std::function<void()>& _Callback);

            // Frenchie::Application::Layer
            virtual bool awake() = 0;

        protected:

            friend class ProcessQueue;

            // info
            std::string           m_Name     = STRINGIFY(Process);
            Frenchie::Core::UUID4 m_UUID;
            bool                  m_Paused   = false;
            bool                  m_Finished = false;
            bool                  m_Canceled = false;
            bool                  m_Failed   = false;

            // callbacks
            std::function<void()> m_OnFinished;
            std::function<void()> m_OnCanceled; 
            std::function<void()> m_OnFailed;

            // synchronization
            mutable std::mutex m_Mutex;

            // service methods
            void finish();
            void invokeOnFinished();
            void invokeOnCanceled();
            void invokeOnFailed();
        };

        class ProcessQueue : public Layer
        {
        public:
            ProcessQueue(){}
            virtual ~ProcessQueue(){}

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
                    if((*it)->finished() || (*it)->canceled() || (*it)->failed())
                    {
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
                    (*it)->cancel();
            }

            virtual bool allows_multiple_instances() const
            {
                return false;
            }

            template<typename Type, typename ... Arguments>
            Frenchie::Core::Reference<Type> push(Arguments... _Parameters)
            {
                m_Queue.push(std::make_shared<Type>(_Parameters...));
                return m_Queue.back();
            }

            std::list<std::shared_ptr<Process>>::const_iterator begin() const
            {
                return m_Executed.begin();
            }

            std::list<std::shared_ptr<Process>>::const_iterator end() const
            {
                return m_Executed.end();
            }

            // static API
            static Frenchie::Core::Reference<ProcessQueue> instance()
            {
                auto layer = Frenchie::Application::application()->find_layer<ProcessQueue>();
                
                if(layer == nullptr) 
                    layer = Frenchie::Application::application()->push_layer<ProcessQueue>();

                return layer;
            }

        protected:

            std::queue<std::shared_ptr<Process>> m_Queue = 
                std::queue<std::shared_ptr<Process>>();

            std::list<std::shared_ptr<Process>> m_Executed =    
                std::list<std::shared_ptr<Process>>();
        };

        class IProcessStatus
        {
        public:
            IProcessStatus(){}
            virtual ~IProcessStatus(){}
            virtual std::string iprocess_status_request_status() = 0;
        };

        class IProcessProgress
        {
        public:
            IProcessProgress(){}
            virtual ~IProcessProgress(){}
            virtual float iprocess_progress_request_progress() = 0;
        };
    }
}