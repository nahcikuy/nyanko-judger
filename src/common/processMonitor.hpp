#ifndef __COMMON_PROCESS_MONITOR
#define __COMMON_PROCESS_MONITOR

#include "headers.hpp"

using namespace std;

struct ProcessStatus
{
    int err = 0;
    int signal = 0;
    int status = 0;
    int cpuTime;
    int realTime;
    int memoryUsage;
};

class ProcessMonitor
{
    int _pid;
    int timeout;
    int maxMemory;
    ProcessStatus processStatus;

public:
    ProcessMonitor(int pid);
    ProcessMonitor(int pid, int timeout, int maxMemory);
    ProcessStatus getStatus();
    bool isTLE();
    bool isMLE();
    bool isOLE();
};

#endif
