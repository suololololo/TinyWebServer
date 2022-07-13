#ifndef __ASYNC_LOGGING_H__
#define __ASYNC_LOGGING_H__

#include "../util/noncopyable.h"
#include "log_stream.h"
#include "../util/mutex.h"
#include "../util/condition.h"
#include <sys/types.h>
#include <vector>
#include "../util/thread.h"
class AsyncLogging : NonCopyAble
{
public:
    AsyncLogging(const char *filename, int flush_internal = 2);
    ~AsyncLogging();
    void Append(const char *logline, int len);
    void ThreadFunc();
    void start()
    {
        running_ = true;
        thread_.start();
        latch_.wait();
    }
    void stop()
    {
        running_ = false;
        cond_.notify();
        thread_.join();
    }

private:
    FixBuffer<kLargeBuffer>::ptr current_buf_;
    FixBuffer<kLargeBuffer>::ptr next_buf_;
    std::vector<FixBuffer<kLargeBuffer>::ptr> buffers_;    
    MutexLock mutex_;
    Condition cond_;
    CountDownLatch latch_;
    const char *filename_;
    Thread thread_;
    const int flush_internal_;
    bool running_;
};

#endif