#include "server.h"

Server::Server(int thread_num)
{
    baseLoop_ = new EventLoop();
    pool_ = new EventLoopThreadPool(thread_num, baseLoop_);
}
Server::~Server()
{
}
void Server::start()
{
    if (pool_)
    {
        pool_->start();
    }
}