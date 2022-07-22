/*
 * @Author: jiajun
 * @Date: 2022-07-16 15:49:35
 * @FilePath: /TinyWebServer/server/eventloopthreadpool.h
 */
#ifndef __EVENT_LOOP_THREAD_POOL_H__
#define __EVENT_LOOP_THREAD_POOL_H__
#include "util/thread.h"
#include "util/noncopyable.h"
#include "server/eventloop.h"
#include "server/eventloopthread.h"
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