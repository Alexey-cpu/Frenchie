// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplicationEditorLauncher.hpp>

class Task
{
public:
    
    void execute()
    {
        while(!m_Finished) 
        {
            // wait untill 
            std::unique_lock<std::mutex> lock(m_Mutex);
            
            m_Wait.wait(
                lock, 
                [this] 
                { 
                    return m_Paused || m_Canceled; 
                }
            );

            if(m_Canceled) break;
        }
    }

protected:
    std::atomic<bool> m_Paused{false};
    std::atomic<bool> m_Canceled{false};
    std::atomic<bool> m_Finished{false};

    std::condition_variable m_Wait;
    std::mutex              m_Mutex;
};

int main(int argc, char *argv[])
{
    return Frenchie::Editor::Launcher::execute();
}