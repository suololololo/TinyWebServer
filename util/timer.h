#ifndef __TIMER_H__
#define __TIMER_H__
#include <memory>
#include <queue>
class HttpData;

class TimerNode
{
public:
    typedef std::shared_ptr<TimerNode> ptr;
    TimerNode(std::shared_ptr<HttpData> data, int timeout);
    ~TimerNode();
    bool isValid();
    void updated(int timeout);
    void clearRequest();
    void setDelete() { deleted_ = true; }
    int getExpiredTime() { return expired_time_; }
    bool isDeleted() {return deleted_;}
private:
    int expired_time_;
    std::shared_ptr<HttpData> data_;
    bool deleted_;
};

struct TimerCmp
{
    bool operator()(std::shared_ptr<TimerNode> &a, std::shared_ptr<TimerNode> &b)
    {
        return a->getExpiredTime() > b->getExpiredTime();
    }
};

class Timer
{
public:
    Timer();
    ~Timer();
    void trick();
    void AddNode(std::shared_ptr<HttpData> data, int timeout);

private:
    std::priority_queue<TimerNode::ptr, std::deque<TimerNode::ptr>, TimerCmp> queue_;
};

#endif