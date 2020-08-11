#ifndef __NJUDGER_RUNNER
#define __NJUDGER_RUNNER

#include "general.hpp"

namespace njudger
{
    void run();
    void fin(int result, int testId);
    void runOnce(string inFile, vector<string> &extFiles, int testId, int testCount);
    void checkerRunner(string srcFile, string inFile, string outFile, vector<string> &extFiles, int &result, string &judgerLog);
}; // namespace njudger

#endif
