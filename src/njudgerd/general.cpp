#include "general.hpp"
#include "../common/utils.hpp"

namespace njudgerd
{
    const string VERSION = "0.0.1";

    Conf conf;
    Logger logger;

    void terminatePro(int code)
    {
        logger.print("Terminated (code = " + itoa(code) + ")");
        exit(code);
    }
} // namespace njudgerd
