#include <FrenchieCoreProcess.hpp>

// Core
#include <FrenchieCoreThreadPool.hpp>

using namespace Frenchie::Core;

Process::Process(const std::string& _Command, const std::string& _Arguments)
{
    m_Pool.enqueue(
        [this, _Command, _Arguments]()
        {
            execute(_Command, _Arguments);
        }
    );
}

Process::~Process()
{
    stop(); // stop process on destroy
}

std::string Process::get_status() const
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_Status.m_Buffer;
}

// Platform dependent code
#ifdef IS_UNIX

// Unix
#include <unistd.h>
#include <signal.h>

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

void Process::execute(const std::string& _Command, const std::string& _Arguments = std::string())
{
    enum PIPE : int
    {
        READ,
        WRITE,
        BOTH
    };

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

#endif

#ifdef IS_WINDOWS

// WINAPI
#include <windows.h>
#include <stdio.h>

void Process::stop()
{
    try
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        PROCESS_INFORMATION pi = std::any_cast<PROCESS_INFORMATION>(m_ProcessInfo);
        TerminateProcess(pi.hProcess, 1);
    }
    catch(const std::exception& e)
    {
        m_Status.push(e.what());
    }
}

void Process::pause()
{
    try
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        PROCESS_INFORMATION pi = std::any_cast<PROCESS_INFORMATION>(m_ProcessInfo);
        DebugActiveProcess(pi.dwProcessId);
    }
    catch(const std::exception& e)
    {
        m_Status.push(e.what());
    }
}

void Process::resume()
{
    try
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        PROCESS_INFORMATION pi = std::any_cast<PROCESS_INFORMATION>(m_ProcessInfo);
        DebugActiveProcessStop(pi.dwProcessId);
    }
    catch(const std::exception& e)
    {
        m_Status.push(e.what());
    }
}

bool Process::alive() const
{
    try
    {
        PROCESS_INFORMATION pi = std::any_cast<PROCESS_INFORMATION>(m_ProcessInfo);
        DWORD exitCode;
        return GetExitCodeProcess(pi.hProcess, &exitCode) && exitCode == STILL_ACTIVE;
    }
    catch(const std::exception& e)
    {
        m_Status.push(e.what());
        return false;
    }
}

bool Process::exited() const
{
    return !alive();
}

bool Process::paused() const
{
    try
    {
        PROCESS_INFORMATION pi = std::any_cast<PROCESS_INFORMATION>(m_ProcessInfo);
        BOOL bDebuggerPresent = false;
        return CheckRemoteDebuggerPresent(pi.hProcess, &bDebuggerPresent) && bDebuggerPresent;
    }
    catch(const std::exception& e)
    {
        m_Status.push(e.what());
        return false;
    }
}

bool Process::stopped() const
{
    return !alive();
}

void Process::execute(const std::string& _Command, const std::string& _Arguments)
{
    HANDLE              hReadPipe;
    HANDLE              hWritePipe;
    SECURITY_ATTRIBUTES sa;
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;

    // Set up security attributes for pipe handles
    sa.nLength              = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle       = TRUE;
    sa.lpSecurityDescriptor = NULL;

    // Create the pipe
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) 
    {
        // Handle error
        return;
    }

    // Ensure the write handle is inheritable
    if (!SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0)) 
    {
        // Handle error
        return;
    }

    // Initialize STARTUPINFO
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb         = sizeof(STARTUPINFO);
    si.dwFlags    = STARTF_USESTDHANDLES;
    si.hStdOutput = hWritePipe;
    si.hStdError  = hWritePipe; // Redirect stderr to the same pipe

    // Create the child process
    if (!CreateProcess(
        NULL,                                                      // No module name (use command line)
        (LPSTR)fmt::format("{} {}", _Command, _Arguments).c_str(), // Command line
        NULL,                                                      // Process security attributes
        NULL,                                                      // Thread security attributes
        TRUE,                                                      // Inherit handles
        0,                                                         // Creation flags
        NULL,                                                      // Environment
        NULL,                                                      // Current directory
        &si,                                                       // STARTUPINFO
        &pi                                                        // PROCESS_INFORMATION
    )) {
        // Handle error
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return;
    }

    m_ProcessInfo = pi;

    // Close the write end of the pipe in the parent process
    CloseHandle(hWritePipe);

    // Read output from the pipe
    CHAR buffer[4096];
    DWORD bytesRead;
    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead != 0) 
    {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer
        m_Status.push(buffer);
        //printf("%s", buffer);
    }

    // Wait for the child process to exit (optional)
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close remaining handles
    CloseHandle(hReadPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

#endif