#include "server/eventloop.h"
#include "server/epoller.h"
#include "log/logging.h"
#include "util/util.h"
#include <pthread.h>
#include <sys/eventfd.h>
#include <iostream>
#include <unistd.h>
// __thread EventLoop *t_loopInThisThread = 0;
int createEventfd()
{
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG << "Failed in eventfd";
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      eventHandling_(false),
      callingPendingFunctors_(false),
      threadId_(CurrentThread::tid()),
      epoller_(new Epoller()),
      wakeupFd_(createEventfd()),
      mutex_(),
      wakeupChannel_(new Channel(this, wakeupFd_))
{
    // if (t_loopInThisThread)
    // {
    // }
    // else
    // {
    //     t_loopInThisThread = this;
    // }

    wakeupChannel_->setEvent(EPOLLIN | EPOLLET);
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->setConnCallback(std::bind(&EventLoop::handleConn, this));
    epoller_->epollAdd(wakeupChannel_, 0);
}
EventLoop::~EventLoop()
{
    assert(!quit_);
    quit();
    // t_loopInThisThread = NULL;
    close(wakeupFd_);
}
void EventLoop::loop()
{
    // 外部调用入口
    assert(!looping_);
    assert(isInLoopThread());
    looping_ = true;
    quit_ = false;
    std::vector<Channel::ptr> activeChannel;
    while (!quit_)
    {
        activeChannel.clear();
        activeChannel = epoller_->epoll();
        eventHandling_ = true;
        for (auto &it : activeChannel)
        {
            // LOG << "Thread id is " << threadId_ << " active event is " << it->getRevent() << " fd is " << it->getFd();
            it->handleEvent();
        }
        eventHandling_ = false;
        doPendingFunctors();
        epoller_->handleExpired();
    }
    looping_ = false;
}
void EventLoop::updateChannel(Channel::ptr channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();

    epoller_->updateChannel(channel);
}

void EventLoop::wakeup()
{
    //  用于唤醒阻塞在 epoll_wait 函数的线程自己
    //  忘wakefd 中写入一个字符
    uint64_t one = 1;
    ssize_t n = writen(wakeupFd_, (char *)(&one), sizeof(one));
    if (n != sizeof(one))
    {
        LOG << "EventLoop::wakeup() writes " << n << "bytes instead of 8";
    }
}

void EventLoop::doPendingFunctors()
{
    // 处理任务的回调 处理epoll过程中的外部事件
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    {
        MutexLockGurad lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for (size_t i = 0; i < functors.size(); ++i)
    {
        functors[i]();

    }
    callingPendingFunctors_ = false;
}

void EventLoop::quit()
{
    quit_ = true;
    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::abortNotInLoopThread()
{
    LOG << "EventLoop::abortNotInLoopThread - EventLoop " << this
        << " was created in threadId_ = " << threadId_
        << ", current thread id = " << CurrentThread::tid();
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = readn(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        LOG << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
    wakeupChannel_->setEvent(EPOLLIN | EPOLLET);
}

void EventLoop::handleConn()
{
    epoller_->epollMod(wakeupChannel_, 0);
}