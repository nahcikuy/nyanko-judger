#include "jobController.hpp"
#include "../common/headers.hpp"
#include "../common/pipe.hpp"

namespace njudgerd
{
    class Worker
    {
        PCI job;
        atomic<bool> terminateFlag;

        void work();
        void getStatus(Pipe out);
        void fin(CJsonObject response);
        void monitor(int pid, string loggerFile, Pipe err);
        bool readLoggerFile(CJsonObject &final, string loggerFile);

        static void __monitor(int pid, string loggerFile, Pipe err, Worker *_this);
        static void __getStatus(Pipe out, Worker *_this);

    public:
        Worker(PCI job);
        static void newWorker(PCI job);
    };
} // namespace njudgerd
