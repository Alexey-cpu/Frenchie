#include <FrenchieCoreProcess.hpp>

// Core
#include <FrenchieCoreThreadPool.hpp>

using namespace Frenchie::Core;

#ifdef IS_UNIX

// Unix
#include <unistd.h>
#include <signal.h>

Process::Process(const std::string& _Command, const std::string& _Arguments)
{
    enum PIPE : int
    {
        READ,
        WRITE,
        BOTH
    };

    m_Pool.enqueue(
        [this, _Command, _Arguments]()
        {
            // open read/write pipe
            int fd[PIPE::BOTH];

            if(pipe(fd) < 0)
            {
                m_Status.push("Could not open pipe");
                return;
            }

            // fork() child process
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                m_PID = fork();

                if (m_PID < 0)
                {
                    m_Status.push("Could start process");
                    return;
                }
            }

            if(m_PID == 0)
            {
                // here we have child code

                // close read end of parent pipe
                close(fd[PIPE::READ]);

                // move out/err to pipe
                dup2(fd[PIPE::WRITE], STDOUT_FILENO);
                dup2(fd[PIPE::WRITE], STDERR_FILENO);

                // execute command with given arguments
                execlp(_Command.c_str(), _Arguments.c_str(), NULL);

                // handle execlp errors
                perror("execvp failed");

                // exit if all is Ok
                exit(1);
            }
            else
            {
                // here we have parent code

                // close parent pipe write end
                close(fd[PIPE::WRITE]);

                // read pipe 
                const int buffSize = 1024;
                char buffer[buffSize];

                while(read(fd[0], buffer, buffSize) > 0)
                {
                    std::lock_guard<std::mutex> lock(m_Mutex);
                    m_Status.push(buffer);
                }

                // wait until finish
                int status = 0;
                waitpid(m_PID, &status, 0);

                // TODO: update status below
                if(WIFEXITED(status))
                {
                    std::lock_guard<std::mutex> lock(m_Mutex);
                    m_Status.push(fmt::format("\nProcess finished...\n"));
                }

                if(WIFSIGNALED(status))
                {
                    std::lock_guard<std::mutex> lock(m_Mutex);
                    m_Status.push(fmt::format("\nProcess stopped by a signal '{}'\n", strsignal(WTERMSIG(status))));
                }

                // clean up
                close(fd[PIPE::READ]);
                close(fd[PIPE::WRITE]);
            }
        }
    );
}

Process::~Process()
{
    stop(); // stop process on destroy
}

// getters
std::string Process::get_status() const
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_Status.m_Buffer;
}

void Process::stop()
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    kill(m_PID, SIGKILL);
}

void Process::pause()
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    kill(m_PID, SIGSTOP);
}

void Process::resume()
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    kill(m_PID, SIGCONT);
}

bool Process::alive() const
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return kill(m_PID, 0) == 0;
}

bool Process::exited() const
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    int status = 0;
    waitpid(m_PID, &status, WNOHANG | WUNTRACED | WCONTINUED);
    return WIFEXITED(status);
}

bool Process::paused() const
{
    // guarantee safe access
    std::lock_guard<std::mutex> lock(m_Mutex);

    // know if a process has been stopped
    bool paused = false;
    int  status = 0;

    if(waitpid(m_PID, &status, WNOHANG | WUNTRACED | WCONTINUED) > 0) 
    {
        if (WIFSTOPPED(status))
            paused = true;
        else if (WIFCONTINUED(status))
            paused = false;
    }

    return paused;
}

bool Process::stopped() const
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    int status = 0;
    return waitpid(m_PID, &status, WNOHANG | WUNTRACED | WCONTINUED) > 0 && WIFSIGNALED(status);
}

#endif