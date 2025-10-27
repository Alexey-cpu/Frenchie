#include <FrenchieCoreProcess.hpp>

// Core
#include <FrenchieCoreThreadPool.hpp>
#include <FrenchieCorePlatform.hpp>

using namespace Frenchie::Core;

// Platform agnostic code
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
    // stop process
    stop();

    // remove process info
    if(m_Info != nullptr) 
        free(m_Info);
}

std::string Process::status() const
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_Status.m_Buffer;
}

// Platform specific code
#ifdef IS_UNIX

struct ProcessInfo
{
    int PID = -1;
};

// Unix
#include <unistd.h>
#include <signal.h>

void Process::stop()
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    kill(reinterpret_cast<ProcessInfo*>(m_Info)->PID, SIGKILL);
}

void Process::pause()
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    kill(reinterpret_cast<ProcessInfo*>(m_Info)->PID, SIGSTOP);
}

void Process::resume()
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    kill(reinterpret_cast<ProcessInfo*>(m_Info)->PID, SIGCONT);
}

bool Process::alive() const
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return kill(reinterpret_cast<ProcessInfo*>(m_Info)->PID, 0) == 0;
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
    int pid = -1;

    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        pid = fork();

        // cache process info
        ProcessInfo* info = (ProcessInfo*)malloc(sizeof(ProcessInfo));
        info->PID = pid;
        m_Info = info;


        if (pid < 0)
        {
            m_Status.push("Could start process");
            return;
        }
    }

    if(pid == 0)
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
        waitpid(pid, &status, 0);

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

struct ProcessInfo
{
    PROCESS_INFORMATION pi;
};

void Process::stop()
{
    TerminateProcess(reinterpret_cast<ProcessInfo*>(m_Info)->pi.hProcess, 1);
}

void Process::pause()
{
    DebugActiveProcess(reinterpret_cast<ProcessInfo*>(m_Info)->pi.dwProcessId);
}

void Process::resume()
{
    DebugActiveProcessStop(reinterpret_cast<ProcessInfo*>(m_Info)->pi.dwProcessId);
}

bool Process::alive() const
{
    DWORD exitCode;
    return GetExitCodeProcess(reinterpret_cast<ProcessInfo*>(m_Info)->pi.hProcess, &exitCode) && 
            exitCode == STILL_ACTIVE;
}

void Process::execute(const std::string& _Command, const std::string& _Arguments)
{
    HANDLE              hReadPipe;
    HANDLE              hWritePipe;
    SECURITY_ATTRIBUTES sa;
    STARTUPINFO         si;

    // allocate process info
    ProcessInfo* process = new ProcessInfo();
    m_Info = process;

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

    char cmd[ MAX_PATH ];
    size_t nSize = _countof(cmd);
    getenv_s(&nSize, cmd, "COMSPEC" );

    // Create the child process
    if (!CreateProcess(
        NULL,                                                                 // No module name (use command line)
        (LPSTR)fmt::format("{} /c {} {}", cmd, _Command, _Arguments).c_str(), // Command line
        NULL,                                                                 // Process security attributes
        NULL,                                                                 // Thread security attributes
        TRUE,                                                                 // Inherit handles
        0,                                                                    // Creation flags
        NULL,                                                                 // Environment
        NULL,                                                                 // Current directory
        &si,                                                                  // STARTUPINFO
        &process->pi                                                          // PROCESS_INFORMATION
    )) {
        // Handle error
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return;
    }

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
    WaitForSingleObject(process->pi.hProcess, INFINITE);

    // Close remaining handles
    CloseHandle(hReadPipe);
    CloseHandle(process->pi.hProcess);
    CloseHandle(process->pi.hThread);
}

#endif