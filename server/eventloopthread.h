#ifndef __EVENT_LOOP_THREAD_H__
#define __EVENT_LOOP_THREAD_H__
#include "eventloop.h"
#include "../util/condition.h"
#include "../util/thread.h"
#include "../util/noncopyable.h"
class EventLoopThread : NonCopyAble
{
public:
    typedef std::shared_ptr<EventLoopThread> ptr;
    EventLoopThread();
    ~EventLoopThread();
    EventLoop *startLoop();

private:
    void threadFunc();
    MutexLock mutex_;
    EventLoop *loop_;
    Condition cond_;
    Thread thread_;
};

#endif