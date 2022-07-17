#include "server.h"
#include "../util/util.h"
Server::Server(EventLoop *loop, int thread_num, int port)
    : baseLoop_(loop),
      threadNum_(thread_num),
      port_(port),
      started_(false),
      pool_(new EventLoopThreadPool(threadNum_, baseLoop_)),
      acceptChannel_(new Channel(baseLoop_)),
      listenfd_(socket_bind_listen(port_))
{
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
    acceptChannel_->setEvent(EPOLLIN | EPOLLOUT);
    acceptChannel_->setWriteCallback(std::bind(&Server::handThisConn, this));
    acceptChannel_->setReadCallback(std::bind(&Server::handNewConn, this));
    baseLoop_->addToPoller(acceptChannel_, 0);
}