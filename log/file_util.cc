#include "file_util.h"

FileUtil::FileUtil(const char *logdir)
{
    f_ = fopen(logdir, "aw");
    setbuffer(f_, buf, sizeof(buf));
}

FileUtil::~FileUtil()
{
    fclose(f_);
}

void FileUtil::Append(const char *log, size_t len)
{
    size_t n = write(log, len);
    size_t remain = len - n;
    while (remain > 0)
    {
        size_t x = write(log + n, remain);
        if (x == 0)
        {
            int err = ferror(f_);
            if (err)
                fprintf(stderr, "AppendFile::append() failed !\n");
            break;
        }
        remain -= x;
        n += x;
    }
}
void FileUtil::flush()
{
    fflush(f_);
}
size_t FileUtil::write(const char *log, const size_t len)
{
    return fwrite_unlocked(log, 1, len, f_);
}