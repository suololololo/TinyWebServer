/*
 * @Author: jiajun
 * @Date: 2022-07-16 15:21:27
 * @FilePath: /TinyWebServer/server/eventloopthread.cc
 */
#include "server/eventloopthread.h"
#include <assert.h>
#include <functional>

EventLoopThread::EventLoopThread()
    : mutex_(),
      cond_(mutex_),
      loop_(NULL),
      thread_(std::bind(&EventLoopThread::threadFunc, this), "EventLoopThread")
{

}
EventLoopThread::~EventLoopThread()
{
    if (loop_ != NULL)
    {
        loop_->quit();
        thread_.join();
    }
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;
    {
        MutexLockGurad lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }
    loop.loop();
    // loop = NULL;
}
EventLoop *EventLoopThread::startLoop()
{
    assert(!thread_.started());
    thread_.start();
    {
        MutexLockGurad lock(mutex_);
        while (loop_ == NULL)
        {
            cond_.wait();
        }
    }
    return loop_;
}
