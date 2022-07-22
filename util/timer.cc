/*
 * @Author: jiajun
 * @Date: 2022-07-14 15:32:55
 * @FilePath: /TinyWebServer/util/timer.cc
 */
#include "util/timer.h"
#include "server/httpdata.h"
#include <sys/time.h>
TimerNode::TimerNode(std::shared_ptr<HttpData> data, int timeout)
    : data_(data),
      deleted_(false)
{
    struct timeval curr;
    gettimeofday(&curr, NULL);
    expired_time_ = (((curr.tv_sec % 10000) * 1000) + (curr.tv_usec / 1000)) + timeout;
}

TimerNode::~TimerNode()
{
    if (data_)
        data_->handleClose();
}
bool TimerNode::isValid()
{
    struct timeval curr;
    gettimeofday(&curr, NULL);
    int now = (((curr.tv_sec % 10000) * 1000) + (curr.tv_usec / 1000));
    if (now < expired_time_)
    {
        return true;
    }
    else
    {
        setDelete();
        return false;
    }
}
void TimerNode::clearRequest()
{
    data_.reset();
    setDelete();
}
void TimerNode::updated(int timeout)
{
    struct timeval curr;
    gettimeofday(&curr, NULL);
    expired_time_ = (((curr.tv_sec % 10000) * 1000) + (curr.tv_usec / 1000)) + timeout;
}

Timer::Timer()
{
}
Timer::~Timer()
{
}
void Timer::trick()
{
    while (!queue_.empty())
    {
        TimerNode::ptr t = queue_.top();
        if (t->isDeleted())
        {
            queue_.pop();
        }
        else if (!t->isValid())
        {
            queue_.pop();
        }
        else
            break;
    }
}
void Timer::AddNode(std::shared_ptr<HttpData> data, int timeout)
{
    TimerNode::ptr node(new TimerNode(data, timeout));
    queue_.push(node);
    data->linkTimer(node);
}