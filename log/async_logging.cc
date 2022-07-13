#include "async_logging.h"
#include "log_file.h"
#include <assert.h>
AsyncLogging::AsyncLogging(const char *filename, int flush_internal)
    : filename_(filename),
      cond_(mutex_),
      current_buf_(new FixBuffer<kLargeBuffer>),
      next_buf_(new FixBuffer<kLargeBuffer>),
      thread_(std::bind(&AsyncLogging::ThreadFunc, this), "logging"),
      mutex_(),
      buffers_(),
      latch_(1),
      flush_internal_(flush_internal),
      running_(true)

{
    current_buf_->bzero();
    next_buf_->bzero();
    buffers_.reserve(16);
}
AsyncLogging::~AsyncLogging()
{
}
void AsyncLogging::Append(const char *logline, int len)
{
    if (current_buf_->Available() > len)
    {
        current_buf_->Append(logline, len);
        return;
    }
    else
    {
        buffers_.push_back(current_buf_);
        current_buf_.reset();
        if (next_buf_)
        {
            current_buf_ = std::move(next_buf_);
        }
        else
        {
            current_buf_.reset(new FixBuffer<kLargeBuffer>);
        }
        current_buf_->Append(logline, len);
        cond_.notify();
    }
}
void AsyncLogging::ThreadFunc()
{
    assert(running_ == true);
    latch_.countDown();
    // 写入底层文件
    FixBuffer<kLargeBuffer>::ptr new_buffer1(new FixBuffer<kLargeBuffer>);
    FixBuffer<kLargeBuffer>::ptr new_buffer2(new FixBuffer<kLargeBuffer>);
    new_buffer1->bzero();
    new_buffer2->bzero();
    std::vector<FixBuffer<kLargeBuffer>::ptr> new_buffers;
    new_buffers.reserve(16);
    LogFile logfile(filename_);
    while (running_)
    {
        assert(new_buffer1 && new_buffer1->length() == 0);
        assert(new_buffer2 && new_buffer2->length() == 0);
        assert(new_buffers.empty());
        {
            MutexLockGurad lock(mutex_);
            if (buffers_.empty())
                cond_.waitForSeconds(flush_internal_);
            buffers_.push_back(current_buf_);
            current_buf_.reset();
            current_buf_ = std::move(new_buffer1);
            new_buffers.swap(buffers_);
            if (!next_buf_)
            {
                next_buf_ = std::move(new_buffer2);
            }
        }
        assert(!new_buffers.empty());
        if (new_buffers.size() > 25)
        {
            new_buffers.erase(new_buffers.begin() + 2, new_buffers.end());
        }
        for (size_t i = 0; i < new_buffers.size(); ++i)
        {
            logfile.Append(new_buffers[i]->get_data(), new_buffers[i]->length());
        }
        if (new_buffers.size() > 2)
        {
            new_buffers.resize(2);
        }
        if (!new_buffer1)
        {
            assert(!new_buffers.empty());
            new_buffer1 = new_buffers.back();
            new_buffers.pop_back();
            new_buffer1->reset();
        }
        if (!new_buffer2)
        {
            assert(!new_buffers.empty());
            new_buffer2 = new_buffers.back();
            new_buffers.pop_back();
            new_buffer2->reset();
        }
        new_buffers.clear();
        logfile.flush();
    }

    logfile.flush();
}