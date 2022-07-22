#include "server/epoller.h"
#include "log/logging.h"
#include "server/httpdata.h"
#include "server/eventloop.h"
const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 10000;
Epoller::Epoller()
    : epollfd_(epoll_create1(EPOLL_CLOEXEC)),
      events_(EVENTSNUM)
{
    assert(epollfd_ > 0);
}
Epoller::~Epoller()
{
}

std::vector<Channel::ptr> Epoller::epoll()
{
    while (true)
    {
        int event_count = epoll_wait(epollfd_, &(*events_.begin()), events_.size(), EPOLLWAIT_TIME);
        if (event_count < 0)
        {
            // LOG << CurrentThread::tid() << "EPOLL ERROR";
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
            // LOG << "active event is " << events_[i].events;
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
        fd2http_[fd] = request->getHolder();
    }
    fd2chan_[fd] = request;
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvent();
    request->equalAndUpdateLastEvents();
    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        LOG << "epoll add error";
        fd2chan_[fd].reset();
    }
}
void Epoller::epollDel(Channel::ptr request)
{
    int fd = request->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getLastEvent();
    if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &event) < 0)
    {
        LOG << "epoll delete error";
    }
    fd2chan_[fd].reset();
    fd2http_[fd].reset();
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
            LOG << "epoll mod error";
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
void Epoller::updateChannel(Channel::ptr request)
{
    int fd = request->getFd();
    fd2chan_[fd] = request;
}
void Epoller::handleExpired()
{
    timer_.trick();
}