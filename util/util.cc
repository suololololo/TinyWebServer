#include "util.h"
#include "log/logging.h"
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <strings.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <string>
#include <signal.h>

const int MAX_BUFF = 4096;
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
                else if (errno == EAGAIN)
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
    // LOG << "server listen fd is " << listenfd;
    return listenfd;
}

int setNonBlocking(int fd)
{
    // 成功返回0， 失败返回-1
    int old_option = fcntl(fd, F_GETFL, 0);
    if (old_option == -1)
        return -1;
    int new_option = old_option | O_NONBLOCK;
    int ret = fcntl(fd, F_SETFL, new_option);
    if (ret == -1)
        return -1;
    return 0;
}

void setSoketNondelay(int fd)
{
    // 禁用nagle算法 不对包进行合并
    int enable = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&enable, sizeof(enable));
}

ssize_t readn(int fd, std::string &buf, bool &zero)
{
    // 从socket 中读数据 当zero为true时，说明有请求但读不到数据
    ssize_t nread = 0;
    ssize_t readsum = 0;
    while (true)
    {
        char buff[MAX_BUFF];
        if ((nread = read(fd, &buff, MAX_BUFF)) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else if (errno == EAGAIN)
            {
                return readsum;
            }
            else
            {
                perror("read error");
                return -1;
            }
        }
        else if (nread == 0)
        {
            zero = true;
            break;
        }

        readsum += nread;
        buf += std::string(buff, buff + nread);
    }
    return readsum;
}

ssize_t writen(int fd, std::string &sbuff)
{
    ssize_t nleft = sbuff.size();
    ssize_t nwrite = 0;
    ssize_t writesum = 0;
    const char *ptr = sbuff.c_str();
    while (nleft > 0)
    {
        if ((nwrite = write(fd, ptr, nleft)) <= 0)
        {
            if (nwrite < 0)
            {
                if (errno == EINTR)
                {
                    nwrite = 0;
                    continue;
                }
                else if (errno == EAGAIN)
                {
                    return writesum;
                }
                else
                {
                    break;
                }
            }
        }
        writesum += nwrite;
        nleft -= nwrite;
        ptr += nwrite;
    }
    if (writesum == static_cast<int>(sbuff.size()))
        sbuff.clear();
    else
        sbuff = sbuff.substr(writesum);
    return writesum;
}

ssize_t readn(int fd, void *buff, size_t n)
{
    ssize_t nleft = n;
    ssize_t nread = 0;
    ssize_t readsum = 0;
    char *ptr = (char *)buff;
    while (nleft > 0)
    {
        if ((nread = read(fd, ptr, nleft)) < 0)
        {
            if (errno == EINTR)
            {
                nread = 0;
            }
            else if (errno == EAGAIN)
            {
                return readsum;
            }
            else
            {
                perror("read error");
                return -1;
            }
        } else if (nread == 0)
        {
            break;
        }

        readsum += nread;
        nleft -= nread;
        ptr += nread;
    }
    return readsum;
}


void handle_for_sigpipe() {
  struct sigaction sa;
  memset(&sa, '\0', sizeof(sa));
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  if (sigaction(SIGPIPE, &sa, NULL)) return;
}