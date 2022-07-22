/*
 * @Author: jiajun
 * @Date: 2022-07-14 16:39:57
 * @FilePath: /TinyWebServer/server/httpdata.h
 */
#ifndef __HTTP_DATA_H__
#define __HTTP_DATA_H__

#include "util/timer.h"
#include "server/channel.h"
#include <map>
#include <unistd.h>
#include <unordered_map>
class EventLoop;

enum ProcessState
{
    STATE_PARSE_URI = 1,
    STATE_PARSE_HEADERS,
    STATE_PARSE_BODY,
    STATE_ANALYSIS,
    STATE_FINISH
};

enum URIState
{
    PARSE_URI_AGAIN = 1,
    PARSE_URI_ERROR,
    PARSE_URI_SUCCESS
};

enum HeaderState
{
    PARSE_HEADER_SUCCESS = 1,
    PARSE_HEADER_AGAIN,
    PARSE_HEADER_ERROR
};
enum AnalysisState
{
    ANALYSIS_SUCCESS = 1,
    ANALYSIS_ERROR
};
enum HTTPVersion
{
    HTTP_10 = 1,
    HTTP_11
};
enum ParseState
{
    H_START = 0,
    H_KEY,
    H_COLON,
    H_SPACES_AFTER_COLON,
    H_VALUE,
    H_CR,
    H_LF,
    H_END_CR,
    H_END_LF
};

enum ConnectionState
{
    H_CONNECTED = 1,
    H_DISCONNECTING,
    H_DISCONNECTED
};
enum HTTPMethod
{
    POST = 1,
    GET,
    HEAD
};

class MimeType
{
private:
    static void init();
    static std::unordered_map<std::string, std::string> mime;
    MimeType();
    MimeType(const MimeType &m);

public:
    static std::string getMime(const std::string &suffix);

private:
    static pthread_once_t once_control;
};

class HttpData : public std::enable_shared_from_this<HttpData>
{
    // 解析http请求
public:
    typedef std::shared_ptr<HttpData> ptr;

public:
    HttpData(EventLoop *loop, int connectfd);
    ~HttpData() { close(connectfd_); }
    void reset();

    void linkTimer(TimerNode::ptr timer)
    {
        timerNode_ = timer;
    }

    std::shared_ptr<Channel> getChannel() { return channel_; }
    EventLoop *getLoop() { return loop_; }
    void newEvent();
    void handleClose();

private:
    void handleRead();
    void handleWrite();
    void handleConn();
    void handleError(int fd, int stateCode, std::string msg);
    void seperateTimer();
    URIState parseURI();
    HeaderState parseHeaders();
    AnalysisState analysisRequest();

private:
    int connectfd_;
    EventLoop *loop_;
    std::shared_ptr<Channel> channel_;
    std::string inBuffer_;
    std::string outBuffer_;
    ConnectionState connectionState_;
    bool error_;
    ProcessState processState_;
    std::map<std::string, std::string> headers_;
    bool keepAlive_;
    int nowRead_; // buf 中现在已读的数量
    HTTPMethod method_;
    std::string filename_;
    HTTPVersion httpVersion_;
    ParseState hState_;
    std::weak_ptr<TimerNode> timerNode_;
    std::string path_;

};

#endif