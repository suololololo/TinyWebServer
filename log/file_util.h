#ifndef __FILE_UTIL_H__
#define __FILE_UTIL_H__
#include "../util/noncopyable.h"
#include <memory>
class FileUtil : NonCopyAble
{
public:
    typedef std::shared_ptr<FileUtil> ptr;
    explicit FileUtil(const char *logdir);
    ~FileUtil();
    void Append(const char *log, const size_t len);
    void flush();

private:
    size_t write(const char *log, const size_t len);

private:
    FILE *f_;
    char buf[64 * 1024];
};

#endif