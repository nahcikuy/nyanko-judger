#include "udpSocket.hpp"
#include "utils.hpp"

int SocketServer::init(int port)
{
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        return errno;

    _port = port;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (sockaddr *)(&serverAddr), sizeof(serverAddr)) == -1)
        return errno;
    return 0;
}

int SocketServer::receiveMessage(string &message, sockaddr_in &from)
{
    for (;;usleep(500))
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        if (select(sockfd + 1, &readfds, NULL, NULL, &timeout) == -1)
            return errno;

        if (FD_ISSET(sockfd, &readfds))
        {
            int numBytes;
            char buffer[maxBuffLen];
            socklen_t fromLen = sizeof(buffer);

            memset(buffer, 0, sizeof(buffer));
            if (numBytes = (recvfrom(sockfd, buffer, maxBuffLen, 0, (sockaddr *)(&from), &fromLen) == -1))
                return errno;

            message = buffer;
            return 0;
        }
    }
}

int SocketServer::receiveMessage(string &message)
{
    sockaddr_in from;
    return receiveMessage(message, from);
}

int SocketServer::sendMessage(string message, string host, int port)
{
    addrinfo *res;
    if (getaddrinfo(host.c_str(), itoa(port).c_str(), NULL, &res))
        return errno;

    for (addrinfo *now = res; now; now = now->ai_next)
    {
        if ((sendto(sockfd, message.c_str(), message.length(), 0, now->ai_addr, sizeof(sockaddr))) != -1)
            return 0;
    }
    return errno;
}
