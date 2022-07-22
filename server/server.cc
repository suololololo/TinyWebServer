/*
 * @Author: jiajun
 * @Date: 2022-07-16 20:37:14
 * @FilePath: /TinyWebServer/server/server.cc
 */
#include "server/server.h"
#include "util/util.h"
#include "log/logging.h"
#include "server/httpdata.h"
#include "server/eventloopthreadpool.h"
#include "server/eventloop.h"
#include "server/channel.h"
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>

Server::Server(EventLoop *loop, int thread_num, int port)
    : baseLoop_(loop),
      threadNum_(thread_num),
      port_(port),
      started_(false),
      pool_(new EventLoopThreadPool(thread_num, loop)),
      acceptChannel_(new Channel(loop)),
      listenfd_(socket_bind_listen(port))
{
    acceptChannel_->setFd(listenfd_);
    handle_for_sigpipe();
    if (setNonBlocking(listenfd_) < 0)
    {
        LOG << "set socket non block failed";
        abort();
    }
}
Server::~Server()
{
}
void Server::start()
{
    assert(!started_);
    started_ = true;
    assert(pool_);
    pool_->start();
    acceptChannel_->setEvent(EPOLLIN | EPOLLET);
    // acceptChannel_->setWriteCallback(std::bind(&Server::handThisConn, this));
    acceptChannel_->setReadCallback(std::bind(&Server::handNewConn, this));
    acceptChannel_->setConnCallback(std::bind(&Server::handThisConn, this));
    baseLoop_->addToPoller(acceptChannel_, 0);
    // LOG << "server start";
}

void Server::handNewConn()
{

    struct sockaddr_in client_address;
    memset(&client_address, 0, sizeof(client_address));
    int connectfd = 0;
    socklen_t length = sizeof(client_address);
    while ((connectfd = accept(listenfd_, (struct sockaddr *)&client_address, &length)) > 0)
    {
        EventLoop *loop = pool_->getNextLoop();
        LOG << "new connection from " << inet_ntoa(client_address.sin_addr) << ":"
            << ntohs(client_address.sin_port);

        // 服务器最大并发数
        if (connectfd >= MAXFDS)
        {
            LOG << "the number of connection exceeds the limit";
            close(connectfd);
            continue;
        }
        if (setNonBlocking(connectfd) < 0)
        {
            LOG << "set connect fd non blocking error";
            // close(connectfd);
            return;
        }
        setSoketNondelay(connectfd);
        // Channel::ptr
        std::shared_ptr<HttpData> httpdata(new HttpData(loop, connectfd));
        httpdata->getChannel()->setHolder(httpdata);
        loop->queueInLoop(std::bind(&HttpData::newEvent, httpdata));
        // LOG << "connect fd :" << connectfd;
    }
    acceptChannel_->setEvent(EPOLLIN | EPOLLET);
}
void Server::handThisConn()
{
    baseLoop_->updatePoller(acceptChannel_);
}