#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "eventloop.h"
#include "../util/noncopyable.h"
#include "httpdata.h"
#include <functional>
#include <memory>
class Channel : NonCopyAble
{
    // 为一个文件描述符绑定回调函数
public:
    typedef std::function<void()> Callback;
    typedef std::shared_ptr<Channel> ptr;
    explicit Channel(EventLoop *loop);
    Channel(EventLoop *loop, int fd);
    void handleEvent(); // 被 EventLoop->loop() 调用 根据revents 调用相应的回调函数
    void setReadCallback(const Callback &cb) { readCallback_ = cb; }
    void setWriteCallback(const Callback &cb) { writeCallback_ = cb; }
    void setErrorCallback(const Callback &cb) { errorCallback_ = cb; }
    void setConnCallback(const Callback &cb) { connCallback_ = cb; }
    int getFd() const { return fd_; }
    __uint32_t getEvent() const { return events_; }
    __uint32_t getRevent() const { return revents_; }
    void setFd(int fd) { fd_ = fd; }
    void setEvent(__uint32_t events) { events_ = events; }
    void setRevent(__uint32_t revents) { revents_ = revents; }
    HttpData::ptr getHolder()
    {
        HttpData::ptr ret(holder_.lock());
        return ret;
    }
    EventLoop *ownerLoop() {return loop_;}
    void handleRead();
    void handleWrite();
    void handleConn();
    bool equalAndUpdateLastEvents()
    {
        bool ret = (lastEvents_ == events_);
        lastEvents_ = events_;
        return ret;
    }
private:
    void update();

private:
    EventLoop *loop_;
    int fd_;
    Callback readCallback_;
    Callback writeCallback_;
    Callback errorCallback_;
    Callback connCallback_;
    __uint32_t events_;  // 关心的IO事件
    __uint32_t revents_; // 目前活动的事件
    __uint32_t lastEvents_;
    // 方便找到上层持有该Channel的对象
    std::weak_ptr<HttpData> holder_;
};

#endif