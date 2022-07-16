#ifndef __EVENT_LOOP_THREAD_POOL_H__
#define __EVENT_LOOP_THREAD_POOL_H__
#include "../util/thread.h"
#include "../util/noncopyable.h"
#include "eventloop.h"
#include "eventloopthread.h"
#include <vector>
class EventLoopThreadPool : NonCopyAble
{
public:
    EventLoopThreadPool(int thread_num, EventLoop *baseLoop);
    ~EventLoopThreadPool();
    void start();
    EventLoop *getNextLoop();

private:
    std::vector<EventLoopThread::ptr> threads_;
    std::vector<EventLoop *> loops_;
    EventLoop *baseLoop_;
    int numThreads_;
    bool started_;
    int next_;
};

#endif