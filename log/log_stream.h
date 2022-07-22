#ifndef __LOG_STREAM_H__
#define __LOG_STREAM_H__
#include "../util/noncopyable.h"
#include <memory>
#include <string.h>
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;
template <int SIZE>
class FixBuffer : NonCopyAble
{
public:
    typedef std::shared_ptr<FixBuffer<SIZE>> ptr;

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

    char *current() const
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

class LogStream : NonCopyAble
{
public:


    LogStream &operator<<(bool v)
    {
        buffer_.Append(v ? "1" : "0", 1);
        return *this;
    }
    LogStream &operator<<(short);
    LogStream &operator<<(unsigned short);
    LogStream &operator<<(long);
    LogStream &operator<<(unsigned long);
    LogStream &operator<<(int);
    LogStream &operator<<(unsigned int);
    LogStream &operator<<(long long);
    LogStream &operator<<(unsigned long long);
    LogStream &operator<<(float v)
    {
        *this << static_cast<double>(v);
        return *this;
    }
    LogStream &operator<<(double);
    LogStream &operator<<(char v)
    {
        buffer_.Append(&v, 1);
        return *this;
    }
    LogStream &operator<<(const char *v)
    {
        if (v)
        {
            buffer_.Append(v, strlen(v));
        }
        else
        {
            buffer_.Append("[NULL]", 6);
        }
        return *this;
    }
    // LogStream &operator<<(const void *v);
    LogStream &operator<<(const std::string &v)
    {
        buffer_.Append(v.c_str(), v.length());
        return *this;
    }
    LogStream &operator<<(const unsigned char *v)
    {
        return operator<<(reinterpret_cast<const char *>(v));
    }
    void Append(const char *data, int len) { buffer_.Append(data, len); }
    const FixBuffer<kSmallBuffer> &buffer() const { return buffer_; }
    void resetBuffer() { buffer_.reset(); }

private:
    template <typename T>
    void formInteger(T);

private:
    FixBuffer<kSmallBuffer> buffer_;
    const int MAX_NUMBERIC_SIZE = 32;
};

#endif