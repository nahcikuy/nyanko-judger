#include "sandBox.hpp"
#include "preRunner.hpp"
#include "../common/utils.hpp"

namespace njudger
{
    /*
        Judgement for Code Length Limit Excceeded.
    */
    void checkSrcFileSize()
    {
        int sz = getFileSize(judgement.srcFile);
        if (sz == -1)
            _logger.Nyan(logger, ERR_GET_FILE_SIZE_ERROR, "Error while getting the size of SRCFILE.");
        if (sz > judgement.codeLengthLimit)
            _logger.addResult(logger, CODE_LENGTH_LIMIT_EXCCEEDED), _logger.Nyan(logger);
    }

    /*
        Compile SRCFILE. (if needed)
    */
    void compile()
    {
        Language currentLang = conf.language[judgement.lang];
        if (!currentLang.needCompile)
            return;

        cout << "Compiling..." << endl;
        judgement.destFile = string(TMP_DIR) + "/" + randomString(15);

        int compileProcess = fork();
        if (compileProcess)
        {
            // parent process
            ProcessMonitor processMonitor(compileProcess, currentLang.maxCompileTime, currentLang.maxCompileMemory);
            ProcessStatus processStatus = processMonitor.getStatus();

            if (processStatus.err)
                _logger.Nyan(logger, ERR_GET_RESOURCE_USAGE_FAILED, "Failed to get resource usage: " + string(strerror(errno)));
            if (processStatus.signal == SIGUSR1)
                exit(0); // NOTHING TO DO HERE.

            if (processMonitor.isTLE())
            {
                _logger.addResult(logger, COMPILATION_ERROR);
                logger.Add("compilerLog", "Compilation time limit excceeded."), _logger.Nyan(logger);
            }
            else if (processMonitor.isMLE())
            {
                _logger.addResult(logger, COMPILATION_ERROR);
                logger.Add("compilerLog", "Compilation memory limit excceeded."), _logger.Nyan(logger);
            }
            else if (processMonitor.isOLE())
            {
                _logger.addResult(logger, COMPILATION_ERROR);
                logger.Add("compilerLog", "Compilation output limit excceeded."), _logger.Nyan(logger);
            }
            else if (processStatus.signal)
            {
                _logger.addResult(logger, COMPILATION_ERROR);
                logger.Add("compilerLog", "The compiler was exited by signal " + itoa(processStatus.signal)), _logger.Nyan(logger);
            }
            else if (processStatus.status)
            {
                _logger.addResult(logger, COMPILATION_ERROR);
                logger.Add("compilerLog", "The compiler was exited with code " + itoa(processStatus.status)), _logger.Nyan(logger);
            }
        }
        else
        {
            // compileProcess
            char *args[1 << 10];
            commandArgsToArray(conf.language[judgement.lang].compileCommand, args);

            SandBox sandBox;
            sandBox.init(args);
            if (sandBox.setResLimit(conf.language[judgement.lang].maxCompileTime,
                                    conf.language[judgement.lang].maxCompileMemory,
                                    conf.language[judgement.lang].maxCompileOutput))
                _logger.Nyan(logger, ERR_SANDBOX_SET_RLIMIT_ERROR, "Failed to set resource limit for sand box", true);

            sandBox.execute();

            _logger.Nyan(logger, ERR_COMPILATION_FAILED, "Error while executing compilation: " + string(strerror(errno)), true);
        }
    }
}; // namespace njudger
