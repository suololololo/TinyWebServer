#include "eventloop.h"
#include "../log/logging.h"
#include <pthread.h>
#include <sys/eventfd.h>
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
void EventLoop::doPendingFunctors()
{
    // 处理任务的回调
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
}
EventLoop::~EventLoop()
{
    assert(!looping_);
    assert(isInLoopThread());
    looping_ = true;
    quit_ = false;
    std::vector<Channel::ptr> activeChannel;
    while (!quit_)
    {
        activeChannel.clear();
        activeChannel = epoller_->epoll(-1);
        eventHandling_ = true;
        for (auto &it : activeChannel)
        {
            it->handleEvent();
        }
        eventHandling_ = false;
        doPendingFunctors();
        epoller_->handleExpired();
    }
}
void EventLoop::loop()
{
}
void EventLoop::updateChannel(Channel::ptr request)
{
}

void EventLoop::wakeup()
{
}