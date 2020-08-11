#ifndef __NJUDGERD_INITIALIZER
#define __NJUDGERD_INITIALIZER

namespace njudgerd
{
    void showHelp();
    void daemonInit();
    void loadConf();
    void argParser(int argc, char **argv);
    void initialize(int argc, char **argv);
} // namespace njudgerd

#endif
