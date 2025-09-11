#pragma once

#include <FrenchieApplication.hpp>

// STL
#include <functional>

namespace Frenchie
{
    namespace Application
    {
        class Process : public Layer
        {
        public:
            Process(
                const std::string&           _Name       = std::string(),
                const std::function<void()>& _OnFinished = nullptr, 
                const std::function<void()>& _OnCanceled = nullptr, 
                const std::function<void()>& _OnFailed   = nullptr);
            virtual ~Process();

            // API
            void pause();
            void resume();
            void cancel();

            bool paused()   const;
            bool canceled() const;
            bool finished() const;
            bool failed()   const;

            void on_finished(const std::function<void()>& _Callback)
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                m_OnFinished = _Callback;
            }

            void on_canceled(const std::function<void()>& _Callback)
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                m_OnCanceled = _Callback;
            }

            void on_failed(const std::function<void()>& _Callback)
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                m_OnFailed = _Callback;
            }


            // Frenchie::Application::Layer
            virtual void frame_finish() override;
            virtual void finish() override;

        protected:

            // info
            bool m_Paused   = false;
            bool m_Finished = false;
            bool m_Canceled = false;
            bool m_Failed   = false;

            // callbacks
            std::function<void()> m_OnFinished;
            std::function<void()> m_OnCanceled; 
            std::function<void()> m_OnFailed;

            // synchronization
            mutable std::mutex m_Mutex;

            void invokeOnFinished();
            void invokeOnCanceled();
            void invokeOnFailed();
        };

        class ProcessQueue : public Layer
        {
        public:
            ProcessQueue(){}
            virtual ~ProcessQueue(){}

            template<typename Type, typename ... Arguments>
            Frenchie::Core::Reference<Type> push(Arguments... _Parameters)
            {
                return Frenchie::Application::application()->push_layer<Type>(_Parameters...);
            }

            // static API
            static Frenchie::Core::Reference<ProcessQueue> instance()
            {
                auto layer = Frenchie::Application::application()->find_layer<ProcessQueue>();
                
                if(layer == nullptr) 
                    layer = Frenchie::Application::application()->push_layer<ProcessQueue>();

                return layer;
            }
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