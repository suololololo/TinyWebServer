#ifndef __HTTP_DATA_H__
#define __HTTP_DATA_H__
#include "../util/timer.h"

class HttpData
{
public:
    typedef std::shared_ptr<HttpData> ptr;
    void handleClose();
    void linkTimer(TimerNode::ptr timer);
};

#endif