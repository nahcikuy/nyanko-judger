#ifndef __COMMON_UDPSOCKET
#define __COMMON_UDPSOCKET

#include "headers.hpp"

class SocketServer
{
    int _port;
    int sockfd = -1;
    int maxBuffLen = 1 << 15;
    timeval timeout = {0, 0};

public:
    int init(int port);
    int receiveMessage(string &message, sockaddr_in &from);
    int receiveMessage(string &message);
    int sendMessage(string message, string host, int port);
};

#endif
