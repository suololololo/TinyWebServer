#ifndef __LOGGING_H__
#define __LOGGING_H__
#include "log_stream.h"

class Logger
{
public:
    Logger(const char *filename, int line);
    ~Logger();
    LogStream &stream() { return impl_.stream_; }
    static void set_log_filename(const char *logfilename_) { logfilename = logfilename_; }
    static const char *get_log_filename() { return logfilename; }
    static const char *logfilename;

private:
    class Impl
    {
    public:
        Impl(const char *filename, int line);
        LogStream stream_;
        const char *basename_;
        int line_;
        void formatTime();
    };

private:
    Impl impl_;
};

#define LOG Logger(__FILE__, __LINE__).stream()

#endif