#include "log_stream.h"
#include <algorithm>
const char digits[] = "9876543210123456789";
const char *zero = digits + 9;

template <typename T>
size_t convert(char buf[], T t)
{
    T i = t;
    char *cur = buf;
    do
    {
        int last = static_cast<int>(i % 10);
        i /= 10;
        *cur++ = zero[last];
    } while (i != 0);
    if (t < 0)
    {
        *cur++ = '-';
    }
    *cur = '\0';
    std::reverse(buf, cur);
    return cur - buf;
}

LogStream &LogStream::operator<<(short v)
{
    *this << static_cast<int>(v);
    return *this;
}
LogStream &LogStream::operator<<(unsigned short v)
{
    *this << static_cast<unsigned int>(v);
    return *this;
}
LogStream &LogStream::operator<<(long v)
{
    formInteger(v);
    return *this;
}
LogStream &LogStream::operator<<(unsigned long v)
{
    formInteger(v);
    return *this;
}
LogStream &LogStream::operator<<(int v)
{
    formInteger(v);
    return *this;
}
LogStream &LogStream::operator<<(unsigned int v)
{
    formInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(long long v)
{
    formInteger(v);
    return *this;
}
LogStream &LogStream::operator<<(unsigned long long v)
{
    formInteger(v);
    return *this;
}
LogStream &LogStream::operator<<(double v)
{
    if (buffer_.Available() >= MAX_NUMBERIC_SIZE)
    {
        int len = snprintf(buffer_.current(), MAX_NUMBERIC_SIZE, "%.12g", v);
        buffer_.Add(len);
    }
    return *this;
}

template <typename T>
void LogStream::formInteger(T t)
{
    if (buffer_.Available() > MAX_NUMBERIC_SIZE)
    {
        size_t len = convert(buffer_.current(), t);
        buffer_.Add(len);
    }
}