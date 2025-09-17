#include <FrenchieApplicationThreadDispatcher.hpp>

// Core
#include <FrenchieCoreThreadPool.hpp>


// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>

using namespace Frenchie::Application;

// add sandbox thread dispatcher view into main menu
namespace Frenchie
{
    namespace Editor
    {
        namespace MainMenu
        {
            class OpenThreadDispatcherViewDebug : 
                public Frenchie::Application::Command::Registry<OpenThreadDispatcherViewDebug, void*>
            {
            public:

                OpenThreadDispatcherViewDebug(void* _Sender = nullptr) : 
                    Frenchie::Application::Command::Registry<OpenThreadDispatcherViewDebug, void*>(_Sender){}
                virtual ~OpenThreadDispatcherViewDebug(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->push_layer<ThreadDispatcherView>()->show();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Debug::ThreadDispatcher");
                }
            };

            const bool openIPCActionRegistry = OpenThreadDispatcherViewDebug::registerFactory();
        }
    }
}

// Thread
Thread::Thread(const std::function<void()> _Worker, const std::string& _Name) : 
    m_Worker(_Worker), m_Name(_Name){}

Thread::~Thread(){}

bool Thread::paused() const
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_Paused;
}

bool Thread::finished() const
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_Finished;
}

void Thread::pause()
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Paused = true;
}

void Thread::Thread::resume()
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Paused = false;
    //m_Wait.notify_one();
}

bool Thread::execute()
{
    Frenchie::Core::ThreadPool::instance()->enqueue(
        [this]()
        {
            while(!m_Finished) 
            {
                // wait untill resumed
                {
                std::unique_lock<std::mutex> lock(m_Mutex);
                m_Wait.wait(lock, [this]()->bool{return !m_Paused;});
                }
                
                // run worker
                if(m_Worker)
                    m_Worker();

                // finish task
                m_Finished = true;
            }
        }
    );

    return true;
}

void Thread::finish()
{
    if(m_OnFinished)
        m_OnFinished();
}

void Thread::on_finished(const std::function<void()>& _Callback)
{
    m_OnFinished = _Callback;
}