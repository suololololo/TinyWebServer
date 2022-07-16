#ifndef __EPOLLER_H__
#define __EPOLLER_H__
#include "eventloop.h"
#include "../util/timer.h"
#include "channel.h"
#include <sys/epoll.h>

class Epoller
{
    // 供 eventLoop 调用
    //  调用epoll_wait 监听文件描述符
public:
    typedef std::shared_ptr<Epoller> ptr;

    Epoller();
    ~Epoller();
    std::vector<Channel::ptr> epoll(int timeouts); //接口
    std::vector<Channel::ptr> getActiveEvents(int event_count);
    void epollAdd(Channel::ptr request, int timeout);
    void epollDel(Channel::ptr request);
    void epollMod(Channel::ptr request, int timeout);
    void addTimer(Channel::ptr request, int timeout);
    void updateChannel(Channel::ptr request);
    void handleExpired();

private:
    static const int MAX_FD = 100000;
    EventLoop *ownerLoop_;
    int epollfd_;
    Timer timer_;
    std::vector<epoll_event> events_;
    Channel::ptr fd2chan_[MAX_FD];
};

#endif