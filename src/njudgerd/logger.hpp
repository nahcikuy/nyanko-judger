#ifndef __NJUDGERD_LOGGER
#define __NJUDGERD_LOGGER

#include "../common/headers.hpp"

namespace njudgerd
{
    class Logger
    {
        mutex loggerMtx; 
    public:
        void print(string message, bool attachErr = false);
    };
} // namespace njudgerd

#endif
