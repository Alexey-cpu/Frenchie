// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplicationEditorLauncher.hpp>

class Task
{
public:

    Task(std::function<void()> _Worker) : 
        m_Worker(_Worker){}
    
    void execute()
    {
        while(!m_Finished) 
        {
            // wait untill resumed
            std::unique_lock<std::mutex> lock(m_Mutex);
            
            m_Wait.wait(lock, [this]()->bool{return m_Paused;});

            // launch worker
            if(m_Worker)
                m_Worker();

            // finish task
            m_Finished = true;
        }

        if(m_OnFinished)
            m_OnFinished();
    }

private:
    std::function<void()> m_Worker    {nullptr};
    std::function<void()> m_OnFinished{nullptr};

    std::atomic<bool>     m_Paused  {false};
    std::atomic<bool>     m_Finished{false};

    std::condition_variable m_Wait;
    std::mutex              m_Mutex;
};

int main(int argc, char *argv[])
{
    return Frenchie::Editor::Launcher::execute();
}