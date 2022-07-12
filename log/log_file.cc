#include "log_file.h"

LogFile::LogFile(const char *filename, int flush_every_n) : flush_every_n_(flush_every_n),
                                                            mutex_(new MutexLock),
                                                            count_(0)
{
    file_util_.reset(new FileUtil(filename));
}
LogFile::~LogFile()
{
}

void LogFile::Append(const char *log, size_t len)
{
    MutexLockGurad lock(*mutex_);
    AppendUnlock(log, len);
}
void LogFile::flush()
{
    MutexLockGurad lock(*mutex_);
    file_util_->flush();
}

void LogFile::AppendUnlock(const char *log, size_t len)
{
    file_util_->Append(log, len);
    count_++;
    if (count_ >= flush_every_n_)
    {
        file_util_->flush();
        count_ = 0;
    }
}