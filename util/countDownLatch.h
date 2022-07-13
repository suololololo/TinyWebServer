#ifndef __COUNT_DOWN_LATCH_H__
#define __COUNT_DOWN_LATCH_H__

#include "noncopyable.h"
#include "mutex.h"
#include "condition.h"
class CountDownLatch : NonCopyAble
{
    /*
    计数器 当计数值为0值， 唤醒阻塞线程
    */
public:
    CountDownLatch(int count) : count_(count), cond_(mutex_)
    {

    }
    void wait()
    {
        MutexLockGurad lock(mutex_);
        while (count_ > 0)
        {
            cond_.wait();
        }
    }
    void countDown()
    {
        MutexLockGurad lock(mutex_);
        --count_;
        if(count_ == 0)
            cond_.notify_all();
    }

private:
    int count_;
    MutexLock mutex_;
    Condition cond_;
};

#endif