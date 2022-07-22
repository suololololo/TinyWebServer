/*
 * @Author: jiajun
 * @Date: 2022-07-15 15:33:21
 * @FilePath: /TinyWebServer/server/eventloop.h
 */
#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__

#include "server/epoller.h"
#include "util/mutex.h"
#include "server/channel.h"
#include "util/CurrentThread.h"
#include "util/timer.h"
#include <vector>
#include <assert.h>
#include <functional>
#include <memory>

class EventLoop
{
    //调用epoller epoll 获取当前活跃的事件， 再通过channel 的handleEvent()函数对事件进行处理
public:
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();
    void loop();
    void updateChannel(Channel::ptr request);
    bool isInLoopThread() { return CurrentThread::tid() == threadId_; } // 是否io线程本身
    void assertInLoopThread() { assert(isInLoopThread()); }
    void wakeup();
    void quit();
    void runInLoop(Functor &&cb)
    {
        if (isInLoopThread())
            cb();
        else
        {
            queueInLoop(std::move(cb));
        }
    }

    void queueInLoop(Functor &&cb)
    {
        {
            MutexLockGurad lock(mutex_);
            pendingFunctors_.emplace_back(std::move(cb));
        }
        if (!isInLoopThread() || callingPendingFunctors_)
        {
            wakeup();
        }
    }

    void addToPoller(Channel::ptr channel, int timeout = 0)
    {
        epoller_->epollAdd(channel, timeout);
    }
    void removeFromPoller(Channel::ptr channel)
    {
        epoller_->epollDel(channel);
    }
    void updatePoller(Channel::ptr channel, int timeout = 0)
    {
        epoller_->epollMod(channel, timeout);
    }

private:
    void abortNotInLoopThread();
    void handleRead();
    void doPendingFunctors();
    void handleConn();
    bool looping_;
    bool quit_;
    bool eventHandling_;
    bool callingPendingFunctors_;
    const pid_t threadId_;
    // std::shared_ptr<Epoller> epoller_;
    // std::shared_ptr<Timer> timer_;
    Epoller::ptr epoller_;
    Timer::ptr timer_;
    int wakeupFd_;
    Channel::ptr wakeupChannel_;
    std::vector<Channel::ptr> channelList_;
    MutexLock mutex_;
    std::vector<Functor> pendingFunctors_;
};

#endif