#include "eventloopthreadpool.h"
#include "../log/logging.h"
#include "eventloopthread.h"
#include <assert.h>
EventLoopThreadPool::EventLoopThreadPool(int thread_num, EventLoop *baseLoop)
    : baseLoop_(baseLoop),
      numThreads_(thread_num),
      started_(false),
      next_(0)
{
    if (numThreads_ <= 0)
    {
        LOG << " Thread nums < 0";
        abort();
    }
}
EventLoopThreadPool::~EventLoopThreadPool()
{
    LOG << "Event Loop Thread pool is destroyed";
}

void EventLoopThreadPool::start()
{
    assert(baseLoop_->isInLoopThread()); // 保证pool内的线程 是由base线程创建的
    started_ = true;
    for (int i = 0; i < numThreads_; ++i)
    {
        EventLoopThread::ptr t(new EventLoopThread);
        threads_.push_back(t);
        loops_.push_back(t->startLoop());
    }
}

EventLoop *EventLoopThreadPool::getNextLoop()
{
    assert(baseLoop_->isInLoopThread());
    EventLoop *loop = baseLoop_;
    if (!loops_.empty())
    {
        loop = loops_[next_];
        next_ = (next_ + 1) % numThreads_;
    }
    return loop;
}
