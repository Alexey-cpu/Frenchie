#include <FrenchieCoreProcess.hpp>

// Core
#include <FrenchieCoreThreadPool.hpp>

using namespace Frenchie::Core;

// Platform independent code
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
    stop();
}

std::string Process::status() const
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

    try
    {
        kill(std::any_cast<int>(m_Info), SIGKILL);
    }
    catch(...)
    {
    }
}

void Process::pause()
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    try
    {
        kill(std::any_cast<int>(m_Info), SIGSTOP);
    }
    catch(...)
    {
    }
}

void Process::resume()
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    
    try
    {
       kill(std::any_cast<int>(m_Info), SIGCONT);
    }
    catch(...)
    {
    }
}

bool Process::alive() const
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    try
    {
        return kill(std::any_cast<int>(m_Info), 0) == 0;
    }
    catch(...)
    {
        return false;
    }
}

void Process::execute(const std::string& _Command, const std::string& _Arguments)
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
    int m_PID = -1;

    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_PID = fork();

        // cache process info
        m_Info = m_PID;

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
        PROCESS_INFORMATION pi = std::any_cast<PROCESS_INFORMATION>(m_Info);
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
        PROCESS_INFORMATION pi = std::any_cast<PROCESS_INFORMATION>(m_Info);
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
        PROCESS_INFORMATION pi = std::any_cast<PROCESS_INFORMATION>(m_Info);
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
        PROCESS_INFORMATION pi = std::any_cast<PROCESS_INFORMATION>(m_Info);
        DWORD exitCode;
        return GetExitCodeProcess(pi.hProcess, &exitCode) && exitCode == STILL_ACTIVE;
    }
    catch(const std::exception& e)
    {
        m_Status.push(e.what());
        return false;
    }
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

    m_Info = pi;

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