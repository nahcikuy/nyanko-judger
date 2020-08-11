#include "sandBox.hpp"

namespace njudger
{
    /*
        Sandbox initialization, where
            argv: the command arguments to execute.
    */
    void SandBox::init(char *argv[])
    {
        int i;
        for (i = 0; argv[i]; i++)
            _argv[i] = argv[i]; // Copying address is OK. No need to use strcpy.
        _argv[i] = NULL;        // IMPORTANT !!
    }

    /*
        Set the resource limit.
        This is a rough limit; thus we may need to check the usage again after running.
    */

    int SandBox::setResLimit(int maxCpuTime, int maxMemory, int maxOutput)
    {
        rlimit timeLimit = {(rlim_t)(maxCpuTime / 1000 + 1), (rlim_t)(maxCpuTime / 1000 + 1)};
        rlimit memoryLimit = {(rlim_t)(1LL * maxMemory * 1024), (rlim_t)(1LL * maxMemory * 1024)};
        rlimit outputLimit = {(rlim_t)maxOutput, (rlim_t)maxOutput};

        errno = 0;
        maxCpuTime &&setrlimit(RLIMIT_CPU, &timeLimit);
        maxMemory &&setrlimit(RLIMIT_AS, &memoryLimit);
        maxOutput &&setrlimit(RLIMIT_FSIZE, &outputLimit);

        return errno;
    }

    /*
        Load seccomp rules for a specified running.
        You may modify this function to define your own seccomp rules.
    */
    int SandBox::loadSeccomp()
    {
        errno = 0;
        scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ALLOW);

        if (!ctx)
            return errno;
        seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(kill), 0);
        seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(fork), 0);
        seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(vfork), 0);
        seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(clone), 0);
        seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(execveat), 0);
        seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(execve), 1, SCMP_A1(SCMP_CMP_NE, (scmp_datum_t)_argv));
        seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(open), 1, SCMP_CMP(1, SCMP_CMP_MASKED_EQ, O_RDWR, O_RDWR));
        seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(openat), 1, SCMP_CMP(2, SCMP_CMP_MASKED_EQ, O_RDWR, O_RDWR));
        seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(open), 1, SCMP_CMP(1, SCMP_CMP_MASKED_EQ, O_WRONLY, O_WRONLY));
        seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(openat), 1, SCMP_CMP(2, SCMP_CMP_MASKED_EQ, O_WRONLY, O_WRONLY));
        seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EACCES), SCMP_SYS(socket), 0);

        if (seccomp_load(ctx))
            return errno;
        return 0;
    }

    /*
        Execute the corresponding command in the sandbox.
        NOTE: This will overwrite the current process and only return when an error occured;
            thus you may need to fork a child process before using the sandbox.
    */
    int SandBox::execute()
    {
        execvp(_argv[0], _argv);
        return errno;
    }
};
