#include "pipe.hpp"

int Pipe::init(int flags)
{
    return pipe2(fd, flags);
}

string Pipe::readStr()
{
    int count = read(fd[0], buffer, 65535);

    if (count > 0)
    {
        buffer[count] = 0;
        return string(buffer);
    }
    else
        return "";
}

void Pipe::redirect(int fd2)
{
    dup2(fd[1], fd2);
}
