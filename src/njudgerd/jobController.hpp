#ifndef __NJUDGERD_JOB_CONTROLLER
#define __NJUDGERD_JOB_CONTROLLER

#define PCI pair<CJsonObject, int>

#include "../common/headers.hpp"

namespace njudgerd
{
    class JobController
    {
        static atomic<int> threadsCount;
        static queue<PCI> jobQueue;
        static mutex jobQueueMtx;

    public:
        static void boss();
        static void finJob();
        static void addJob(PCI job);
    };
} // namespace njudgerd

#endif
