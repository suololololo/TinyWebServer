#include "logging.h"
#include "async_logging.h"
#include <sys/time.h>
#include <iostream>
static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static AsyncLogging *AsyncLogger_;
// const char * Logger::logfilename = "./WebServer.log";
const char * Logger::logfilename = "./1.log";
void once_init()
{
    AsyncLogger_ = new AsyncLogging(Logger::get_log_filename());
    AsyncLogger_->start();
}

void output(const char *msg, int len)
{
    pthread_once(&once_control_, once_init);
    AsyncLogger_->Append(msg, len);
}
Logger::Logger(const char *filename, int line) : impl_(filename, line)
{
}
Logger::~Logger()
{
    // 销毁对象时， 调用async logging 写入
    impl_.stream_ << " -- " << impl_.basename_ << ':' << impl_.line_ << '\n';
    const FixBuffer<kSmallBuffer> &buf(stream().buffer());
    output(buf.get_data(), buf.length());
    std::cout  << "here" << std::endl;
}

Logger::Impl::Impl(const char *filename, int line) : basename_(filename), line_(line)
{
    formatTime();
}

void Logger::Impl::formatTime()
{
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday (&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);   
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
    stream_ << str_t;
}