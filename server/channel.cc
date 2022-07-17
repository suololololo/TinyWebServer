#include "channel.h"

Channel::Channel(EventLoop *loop)
    : fd_(0),
      loop_(loop),
      events_(0),
      lastEvents_(0),
      revents_(0)
{
}
Channel::Channel(EventLoop *loop, int fd)
    : fd_(fd),
      loop_(loop),
      events_(0),
      lastEvents_(0),
      revents_(0)
{
}
void Channel::handleEvent()
{
    events_ = 0;
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        events_ = 0;
        return;
    }
    if (revents_ & EPOLLERR)
    {
        if (errorCallback_)
            errorCallback_();
        events_ = 0;
        return;
    }
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
    {
        handleRead();
    }
    if (revents_ & EPOLLOUT)
    {
        handleWrite();
    }
    handleConn();
}

void Channel::update()
{
}

void Channel::handleRead()
{
    if (readCallback_)
        readCallback_();
}
void Channel::handleWrite()
{
    if (writeCallback_)
        writeCallback_();
}
void Channel::handleConn()
{
    if (connCallback_)
        connCallback_;
}
