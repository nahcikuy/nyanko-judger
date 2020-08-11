#ifndef __NJUDGER_INITIALIZER
#define __NJUDGER_INITIALIZER

#include "general.hpp"
#include "../common/confLoader.hpp"

namespace njudger
{
    void showHelp();
    void argParser(int argc, char **argv);
    void loadConf();
    void paramChecker();
    void initLogger();
    void initialize(int argc, char **argv);
}; // namespace njudger

#endif
