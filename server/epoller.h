/*
 * @Author: jiajun
 * @Date: 2022-07-15 16:32:23
 * @FilePath: /TinyWebServer/server/epoller.h
 */
#ifndef __MYEPOLLER_H__
#define __MYEPOLLER_H__
#include "server/channel.h"

#include "util/timer.h"

#include <sys/epoll.h>
class EventLoop;
class Epoller
{
    // 供 eventLoop 调用
    //  调用epoll_wait 监听文件描述符
public:
    typedef std::shared_ptr<Epoller> ptr;

    Epoller();
    ~Epoller();
    std::vector<Channel::ptr> epoll(); //接口
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
    std::shared_ptr<HttpData> fd2http_[MAX_FD];
};

#endif