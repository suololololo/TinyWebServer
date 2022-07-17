#ifndef __SERVER_H__
#define __SERVER_H__

#include "eventloopthreadpool.h"
#include "../util/noncopyable.h"
class Server : NonCopyAble
{
public:
    Server(EventLoop *loop, int thread_num, int port);
    ~Server();
    void start();
    void handNewConn();
    void handThisConn();
private:
    std::unique_ptr<EventLoopThreadPool> pool_;
    EventLoop *baseLoop_;
    Channel::ptr acceptChannel_;
    int threadNum_;
    int listenfd_;
    int port_;
    bool started_;
};

#endif