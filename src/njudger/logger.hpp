#ifndef __NJUGDER_LOGGER
#define __NJUDGER_LOGGER

#include "../common/headers.hpp"

namespace njudger
{
    class Logger
    {
        string results[11] = {"Accepted", "Wrong Answer", "Presentation Error",
                              "Time Limit Excceeded", "Memory Limit Excceeded",
                              "Output Limit Excceeded", "Code Length Limit Excceeded",
                              "Runtime Error", "Compilation Error", "Checker Error", "System Error"};
        FILE *loggerFile;

    public:
        int init(string loggerFileStr);
        void addResult(CJsonObject &logger, int resultCode, string judgerLog);
        void addResult(CJsonObject &logger, int resultCode);
        void addResult(CJsonObject &logger, int resultCode, int testId);
        void Nyan(CJsonObject &logger, int exitCode, string errorMessage, bool sendSIGUSR1 = false);
        void Nyan(CJsonObject &logger);
    };
} // namespace njudger

#endif
