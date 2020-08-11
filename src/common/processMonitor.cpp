#include "processMonitor.hpp"

ProcessMonitor::ProcessMonitor(int pid) : _pid(pid) {}

void timer(int pid, int timeout)
{
    usleep((timeout + 100) * 1000);
    kill(pid, SIGKILL);
}

/*
    If timeout is set, ProcessMonitor will start a timer thread.
    The timer will kill the monitored process after **(timeout+100)** milliseconds.
    Can't use SIGTERM; it will make external time cost.
    Canceling the thread may cause unexpected results; thus here we don't cancel the timer even if
    the process to monitor has already exited. 
*/

ProcessMonitor::ProcessMonitor(int pid, int timeout, int maxMemory) : _pid(pid), timeout(timeout), maxMemory(maxMemory)
{
    if (timeout)
    {
        thread _timer(timer, pid, timeout);
        _timer.detach();
    }
}

/*
    Wait for the process to terminate,
    and get the status as well as the resource usage.
*/
ProcessStatus ProcessMonitor::getStatus()
{
    int wstatus;
    struct rusage rusage;

    timeval start, end;
    gettimeofday(&start, NULL);

    if (wait4(_pid, &wstatus, 0, &rusage) == -1)
        processStatus.err = errno;
    else
    {
        if (WIFSIGNALED(wstatus))
        {
            processStatus.signal = WTERMSIG(wstatus);
            if (processStatus.signal == SIGKILL)
                processStatus.status = -1; // timeout
            else
                processStatus.status = -2; // OLE, MLE, TLE, etc..
        }
        else if (WIFEXITED(wstatus))
            processStatus.status = WEXITSTATUS(wstatus);
        else                    // ???
            assert(1 + 1 == 3); // ???

        gettimeofday(&end, NULL);
        processStatus.cpuTime = min(long(timeout), (rusage.ru_utime.tv_sec + rusage.ru_stime.tv_sec) * 1000 + (rusage.ru_utime.tv_usec + rusage.ru_stime.tv_usec) / 1000);
        processStatus.realTime = min(long(timeout), (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000);
        processStatus.memoryUsage = min(long(maxMemory), rusage.ru_maxrss);
    }
    return processStatus;
}

/*
    These functions should be called after getStatus().
*/

bool ProcessMonitor::isTLE()
{
    return processStatus.status == -1 || processStatus.cpuTime == timeout;
}

bool ProcessMonitor::isMLE()
{
    return processStatus.memoryUsage == maxMemory;
}

bool ProcessMonitor::isOLE()
{
    return processStatus.signal == SIGXFSZ;
}
