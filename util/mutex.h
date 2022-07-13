#ifndef __MUTEXT_H__
#define __MUTEXT_H__
#include "noncopyable.h"
#include <pthread.h>
#include <memory>
class MutexLock : NonCopyAble
{
public:
    typedef std::shared_ptr<MutexLock> ptr;
    MutexLock()
    {
        pthread_mutex_init(&mutex_, NULL);
    }

    ~MutexLock()
    {
        pthread_mutex_lock(&mutex_);
        pthread_mutex_destroy(&mutex_);
    }

    void Lock()
    {
        pthread_mutex_lock(&mutex_);
    }

    void Unlock()
    {
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t *GetMutex() { return &mutex_; }

private:
    pthread_mutex_t mutex_;
};

class MutexLockGurad : NonCopyAble
{
public:
    typedef std::shared_ptr<MutexLockGurad> ptr;
    explicit MutexLockGurad(MutexLock &lock) : lock_(lock)
    {
        lock_.Lock();
    }
    ~MutexLockGurad()
    {
        lock_.Unlock();
    }

private:
    MutexLock &lock_;
};

#endif