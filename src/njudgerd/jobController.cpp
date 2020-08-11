#include "general.hpp"
#include "server.hpp"
#include "worker.hpp"
#include "jobController.hpp"
#include "../common/pipe.hpp"
#include "../common/utils.hpp"
#include "../common/processMonitor.hpp"

namespace njudgerd
{
    atomic<int> JobController::threadsCount(0);
    queue<PCI> JobController::jobQueue;
    mutex JobController::jobQueueMtx;

    void JobController::finJob()
    {
        threadsCount--;
    }

    void JobController::addJob(PCI job)
    {
        jobQueueMtx.lock();
        if (jobQueue.size() >= conf.jobs.maxCount)
        {
            CJsonObject response("{}");
            response.Add("type", "general-response");
            response.Add("information", "The limit of the number of judgement requests in queue excceeded."
                                        " The request(ID = " +
                                            itoa(job.second) + ") will not be proceeded.");

            thread _response(Server::response, response, job.second);
            _response.detach();
        }
        else
            jobQueue.push(job);
        jobQueueMtx.unlock();
    }

    // The _jobController thread
    void JobController::boss()
    {
        for (;; usleep(20000))
        {
            if (jobQueueMtx.try_lock())
            {
                if (jobQueue.size() && threadsCount < conf.jobs.maxThreads)
                {
                    thread worker(Worker::newWorker, jobQueue.front());
                    threadsCount++, worker.detach(), jobQueue.pop();
                }
                jobQueueMtx.unlock();
            }
        }
    }
} // namespace njudgerd
