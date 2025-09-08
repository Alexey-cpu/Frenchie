#pragma once

#include <FrenchieApplication.hpp>

// STL
#include <functional>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            namespace Async
            {
                class Process : public Layer
                {
                public:
                    Process(
                        const std::function<void()>& _OnFinished, 
                        const std::function<void()>& _OnCanceled, 
                        const std::function<void()>& _OnFailed);
                    virtual ~Process();

                    // API
                    void pause();
                    void resume();
                    void cancel();

                    bool paused()   const;
                    bool canceled() const;
                    bool finished() const;
                    bool failed()   const;

                    // Frenchie::Application::Layer
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
    }
}