#include "epoller.h"
#include "../log/logging.h"
#include "httpdata.h"

const int EVENTSNUM = 4096;
Epoller::Epoller()
    : epollfd_(epoll_create1(EPOLL_CLOEXEC)),
      events_(EVENTSNUM)
{
    assert(epollfd_ > 0);
}
Epoller::~Epoller()
{
}

std::vector<Channel::ptr> Epoller::epoll(int timeouts)
{
    while (true)
    {
        int event_count = epoll_wait(epollfd_, &*events_.begin(), events_.size(), timeouts);
        if (event_count < 0)
        {
            LOG << "EPOLL ERROR";
        }
        else
        {
            std::vector<Channel::ptr> res = getActiveEvents(event_count);
            if (res.size() > 0)
                return res;
        }
    }
}

std::vector<Channel::ptr> Epoller::getActiveEvents(int event_count)
{
    std::vector<Channel::ptr> res;
    for (int i = 0; i < event_count; ++i)
    {
        int fd = events_[i].data.fd;
        Channel::ptr cur = fd2chan_[fd];

        if (cur)
        {
            cur->setRevent(events_[i].events);
            cur->setEvent(0);
            res.push_back(cur);
        }
        else
        {
            LOG << "invalid request! According to this fd, do not exist correspond event";
        }
    }
    return res;
}

void Epoller::epollAdd(Channel::ptr request, int timeout)
{
    int fd = request->getFd();
    if (timeout > 0)
    {
        addTimer(request, timeout);
    }
    fd2chan_[fd] = request;
}
void Epoller::epollDel(Channel::ptr request)
{
    int fd = request->getFd();
    Channel::ptr chan = fd2chan_[fd];
    if (chan)
    {
        fd2chan_[fd].reset();
    }
}
void Epoller::epollMod(Channel::ptr request, int timeout)
{
    if (timeout > 0)
        addTimer(request, timeout);
    int fd = request->getFd();
    if (!request->equalAndUpdateLastEvents())
    {
        struct epoll_event event;
        event.data.fd = fd;
        event.events = request->getEvent();
        if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event) < 0)
        {
            perror("epoll mod error");
            fd2chan_[fd].reset();
        }
    }
}

void Epoller::addTimer(Channel::ptr request, int timeout)
{
    HttpData::ptr ret = request->getHolder();
    if (ret)
    {
        timer_.AddNode(ret, timeout);
    }
    else
    {
        LOG << "timer add fail";
    }
}