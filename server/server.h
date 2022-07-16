#ifndef __SERVER_H__
#define __SERVER_H__

#include "eventloopthreadpool.h"
#include "../util/noncopyable.h"
class Server : NonCopyAble
{
public:
    Server(int thread_num);
    ~Server();
    void start();
private:
    EventLoopThreadPool *pool_;
    EventLoop *baseLoop_;
};

#endif