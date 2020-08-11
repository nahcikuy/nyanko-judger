#ifndef __PIPE
#define __PIPE

#include "headers.hpp"

class Pipe
{
    int fd[2];
    char buffer[1 << 16];

public:
    int init(int flags = 0);
    string readStr();
    void redirect(int fd2);
};

#endif
