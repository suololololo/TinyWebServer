#ifndef __LOG_FILE_H__
#define __LOG_FILE_H__

#include "../util/noncopyable.h"
#include "../util/mutex.h"
#include "file_util.h"
#include <stdio.h>
class LogFile : NonCopyAble
{
    /*
    线程安全版本的 FileUtil 负责日志的底层写入
    每N次写入后 会刷新到文件中
    */
public:
    typedef std::shared_ptr<LogFile> ptr;

    explicit LogFile(const char *filename, int flush_every_n = 1024);
    ~LogFile();
    void Append(const char *log, size_t len);
    void flush();

private:
    void AppendUnlock(const char *log, size_t len);

private:
    FILE *f_;
    MutexLock::ptr mutex_;
    FileUtil::ptr file_util_;
    int flush_every_n_;
    int count_;
};

#endif