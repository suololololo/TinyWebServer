#ifndef __CONDITION_H__
#define __CONDITION_H__
#include "mutex.h"
#include "noncopyable.h"
#include <time.h>
#include <errno.h>
class Condition : NonCopyAble
{
public:
    explicit Condition(MutexLock &mutex) : mutex_(mutex)
    {
        pthread_cond_init(&cond_, NULL);
    }
    ~Condition()
    {
        pthread_cond_destroy(&cond_);
    }
    void wait()
    {
        pthread_cond_wait(&cond_, mutex_.GetMutex());
    }
    void notify()
    {
        pthread_cond_signal(&cond_);
    }
    void notify_all()
    {
        pthread_cond_broadcast(&cond_);
    }
    bool waitForSeconds(int seconds)
    {
        struct timespec t;
        clock_gettime(CLOCK_REALTIME, &t);
        t.tv_sec += static_cast<time_t>(seconds);
        return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex_.GetMutex(), &t);
    }

private:
    pthread_cond_t cond_;
    MutexLock &mutex_;
};

#endif