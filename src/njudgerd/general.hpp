#ifndef __NJUDGERD_GENERAL
#define __NJUDGERD_GENERAL

#include "../common/general.hpp"

#include "logger.hpp"

namespace njudgerd
{
    extern const string VERSION;
    extern struct Conf
    {
        bool daemon = false;
        struct
        {
            int port;
        } server;
        struct
        {
            string host;
            int port;
        } listener;
        struct
        {
            int maxCount;
            int maxThreads;
        } jobs;
        struct
        {
            string location;
            int timeLimit = 900000; // 15 mins
        } njudger;
    } conf;

    extern Logger logger;

    void terminatePro(int code);
} // namespace njudgerd

#endif
