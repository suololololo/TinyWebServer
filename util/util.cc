#include "util.h"
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <strings.h>
#include <netinet/in.h>
ssize_t writen(int fd, void *buf, size_t n)
{
    char *ptr = (char *)(buf);
    ssize_t nwritten = 0;
    ssize_t writtensum = 0;
    size_t nleft = n;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if (nwritten < 0)
            {
                if (errno == EINTR)
                {
                    nwritten = 0;
                    continue;
                }
                else if (errno = EAGAIN)
                {
                    return writtensum;
                }
                else
                {
                    return -1;
                }
            }
        }
        writtensum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return writtensum;
}

int socket_bind_listen(int port)
{
    if (port < 0 || port > 65535)
        return -1;
    int listenfd = 0;
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    // 消除bind时"Address already in use"错误
    int optval = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval,
                   sizeof(optval)) == -1)
    {
        close(listenfd);
        return -1;
    }

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_port = htons(port);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listenfd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        close(listenfd);
        return -1;
    }

    if (listen(listenfd, 2048) == -1)
    {
        close(listenfd);
        return -1;
    }
    if (listenfd == -1)
    {
        close(listenfd);
        return -1;
    }
    return listenfd;
}