/*
 * @Author: jiajun
 * @Date: 2022-07-16 16:56:29
 * @FilePath: /TinyWebServer/server/server.h
 */
#ifndef __SERVER_H__
#define __SERVER_H__
// #include "util/noncopyable.h"
// #include "server/channel.h"
#include <memory>

class EventLoopThreadPool;
class EventLoop;
class Channel;
class Server 
{
public:
    Server(EventLoop *loop, int thread_num, int port);
    ~Server();
    void start();
    void handNewConn();
    void handThisConn();

private:
    std::unique_ptr<EventLoopThreadPool> pool_;
    EventLoop *baseLoop_;
    std::shared_ptr<Channel> acceptChannel_;
    int threadNum_;
    int listenfd_;
    int port_;
    bool started_;
    static const int MAXFDS = 100000;
};

#endif