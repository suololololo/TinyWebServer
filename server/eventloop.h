#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__

#include "../util/mutex.h"
#include "channel.h"
#include "epoller.h"
#include "../util/CurrentThread.h"
#include <vector>
#include <assert.h>
class EventLoop
{
    //调用epoller epoll 获取当前活跃的事件， 再通过channel 的handleEvent()函数对事件进行处理
public:
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();
    void loop();
    void updateChannel(Channel::ptr request);
    bool isInLoopThread(){return CurrentThread::tid() == threadId_;} // 是否io线程本身 
    void assertInLoopThread() {assert(isInLoopThread());}
    void wakeup();
    void quit();
    void runInLoop(const Functor &cb)
    {
        if (isInLoopThread())
            cb();
        else
        {
            queueInLoop(cb);
        }
    }

    void queueInLoop(const Functor &cb)
    {
        {
            MutexLockGurad lock(mutex_);
            pendingFunctors_.push_back(cb);
        }
        if (!isInLoopThread() || callingPendingFunctors_)
        {
            wakeup();
        }
    }

private:
    void abortNotInLoopThread();
    void handleRead();
    void doPendingFunctors();

    bool looping_;
    bool quit_;
    bool eventHandling_;
    bool callingPendingFunctors_;
    const pid_t threadId_;
    Epoller::ptr epoller_;
    Timer::ptr timer_;
    int wakeupFd_;
    Channel::ptr wakeupChannel_;
    std::vector<Channel::ptr> channelList_;
    MutexLock mutex_;
    std::vector<Functor> pendingFunctors_;
};

#endif