#include "worker.hpp"
#include "server.hpp"
#include "general.hpp"
#include "../common/utils.hpp"
#include "../common/processMonitor.hpp"

namespace njudgerd
{
    Worker::Worker(PCI job) : job(job), terminateFlag(false) {}

    // thread interface
    void Worker::newWorker(PCI job)
    {
        Worker worker(job);
        worker.work();
    }
    void Worker::__getStatus(Pipe out, Worker *_this)
    {
        _this->getStatus(out);
    }
    void Worker::__monitor(int pid, string loggerFile, Pipe err, Worker *_this)
    {
        _this->monitor(pid, loggerFile, err);
    }
    // thread interface end

    void Worker::work()
    {
        /*
            Generate execution arguments.
        */
        char *argv[10];
        for (int i = 0; i < 10; i++)
            argv[i] = (char *)malloc(255);

        CJsonObject judgement = job.first["judgement"];
        int language = 0, timeLimit = 0, memoryLimit = 0, codeLengthLimit = 0;
        string checker = "", srcFile, dataDir, loggerFile = TMP_DIR + "/" + randomString(15);

        judgement.Get("checker", checker);
        judgement.Get("srcFile", srcFile);
        judgement.Get("dataDir", dataDir);
        judgement.Get("language", language);
        judgement.Get("timeLimit", timeLimit);
        judgement.Get("memoryLimit", memoryLimit);
        judgement.Get("codeLengthLimit", codeLengthLimit);

        timeLimit = timeLimit ? timeLimit : 1000;
        memoryLimit = memoryLimit ? memoryLimit : 131072;
        codeLengthLimit = codeLengthLimit ? codeLengthLimit : 65535;
        checker = checker == "" ? "default" : checker;

        strcpy(argv[0], conf.njudger.location.c_str());
        strcpy(argv[1], (string("--checker=") + checker).c_str());
        strcpy(argv[2], (string("--language=") + itoa(language)).c_str());
        strcpy(argv[3], (string("--time-limit=") + itoa(timeLimit)).c_str());
        strcpy(argv[4], (string("--memory-limit=") + itoa(memoryLimit)).c_str());
        strcpy(argv[5], (string("--code-length-limit=" + itoa(codeLengthLimit)).c_str()));

        strcpy(argv[6], srcFile.c_str());
        strcpy(argv[7], dataDir.c_str());
        strcpy(argv[8], loggerFile.c_str());

        argv[9] = NULL;

        for(int i=0;argv[i];i++)
            cout<<argv[i]<<" ";
        cout<<endl;
        /*
            Create pipe to obtain information from child process
        */
        Pipe out, err;
        out.init(O_NONBLOCK), err.init(O_NONBLOCK);

        /*
            Create njudger process.
        */
        int njudgerProcess = fork();
        if (njudgerProcess)
        {
            // parent process
            thread _status(__getStatus, out, this);
            thread _monitor(__monitor, njudgerProcess, loggerFile, err, this);
            _status.join(), _monitor.join(), JobController::finJob();
        }
        else
        {
            // njudgerProcess
            // this may be thread-unsafe because of the dup2() call,
            // but I am not sure. Maybe using pthread_atfork() is better?
            out.redirect(1), err.redirect(2);
            execvp(argv[0], argv);

            // cannot output anything here as we have called dup2()
            // use SIGUSR1 to identify this case of error
            raise(SIGUSR1);
        }
    }

    void Worker::getStatus(Pipe out)
    {
        for (int flag = 0; flag < 2; usleep(20000))
        {
            if (terminateFlag == true)
                flag++;
            string statusMessage = out.readStr();
            if (statusMessage != "")
            {
                CJsonObject status("{}");
                status.Add("type", "judgement-status");
                status.Add("status", statusMessage);
                status.Add("timestamp", getTimestamp());
                fin(status);
            }
        }
    }

    void Worker::monitor(int pid, string loggerFile, Pipe err)
    {
        CJsonObject error("{}"), final("{}");
        error.Add("type", "judgement-error-log");
        final.Add("type", "judgement-result");

        ProcessMonitor processMonitor(pid, conf.njudger.timeLimit, 0);
        ProcessStatus processStatus = processMonitor.getStatus();
        terminateFlag = true;

        int exitCode;
        string errorMessage = "";
        string internalError = "";
        string errorLog = err.readStr();

        if (processStatus.err)
        {
            exitCode = -4;
            internalError = "Error while obtaining information from njudger";
        }
        if (processStatus.signal == SIGUSR1)
        {
            exitCode = -3;
            internalError = "Cannot execute njudger";
        }
        else if (processStatus.signal)
        {
            exitCode = -2;
            internalError = "njudger was crashed by signal " + itoa(processStatus.signal);
        }
        else
        {
            exitCode = processStatus.status;
            if (readLoggerFile(final, loggerFile))
                final["judgement"].Get("errorMessage", errorMessage);
            else
            {
                if (internalError == "")
                    internalError = "Can't read logger file";
                errorMessage = internalError + ". This information is generated by njudgerd.";
            }
        }

        final.Add("exitCode", exitCode);
        final.Add("errorMessage", errorMessage);
        final.Add("timestamp", getTimestamp());
        error.Add("exitCode", exitCode);
        error.Add("errorLog", errorLog);
        fin(final), fin(error);
    }

    bool Worker::readLoggerFile(CJsonObject &final, string loggerFile)
    {
        FILE *_log = fopen(loggerFile.c_str(), "r");

        if (!_log)
            return false;

        char buffer[1 << 15];
        memset(buffer, 0, sizeof(buffer));
        fread(buffer, 1 << 15, 1, _log), fclose(_log);
        CJsonObject judgerLogger(buffer);

        if (judgerLogger.GetErrMsg() == "")
        {
            final.Add("judgerLogger", judgerLogger);
            return true;
        }
        else
            return false;
    }

    void Worker::fin(CJsonObject response)
    {
        response.Add("information", job.first["information"]);
        thread _response(Server::response, response.ToFormattedString(), job.second);
        _response.detach();
    }

} // namespace njudgerd
