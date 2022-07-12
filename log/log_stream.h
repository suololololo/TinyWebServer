#ifndef __LOG_STREAM_H__
#define __LOG_STREAM_H__
#include "util/noncopyable.h"
#include <memory>

template <int SIZE>
class FixBuffer : NonCopyAble
{
public:
    typedef std::shared_ptr<FixBuffer> ptr;

    FixBuffer() : cur_(data_) {}
    ~FixBuffer() {}
    void Append(const char *log, size_t len)
    {
        if (Available() > static_cast<int>(len))
        {
            memcpy(cur_, log, len);
            cur_ += len;
        }
    }

    int Available() const
    {
        return static_cast<int>(End() - cur_);
    }

    void reset()
    {
        cur_ = data_;
    }
    void bzero()
    {
        memset(data_, 0, sizeof(data_));
    }
    void Add(size_t len)
    {
        cur_ += len;
    }

    const char *get_data() const
    {
        return data_;
    }

    const char *current() const 
    {
        return cur_;
    }

    int length() const 
    {
        return static_cast<int>(cur_ - data_);
    }

private:
    const char *End() const { return data_ + sizeof(data_); }
    char data_[SIZE];
    char *cur_;
};

#endif