
#include "server/httpdata.h"
#include "server/eventloop.h"
#include "log/logging.h"
#include "util/util.h"
#include <memory>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

pthread_once_t MimeType::once_control = PTHREAD_ONCE_INIT;
std::unordered_map<std::string, std::string> MimeType::mime;

const __uint32_t DEFAULT_EVENT = EPOLLIN | EPOLLET | EPOLLONESHOT;
const int DEFAULT_EXPIRED_TIME = 2000;             // ms
const int DEFAULT_KEEP_ALIVE_TIME = 5 * 60 * 1000; // ms

char favicon[555] = {
    '\x89',
    'P',
    'N',
    'G',
    '\xD',
    '\xA',
    '\x1A',
    '\xA',
    '\x0',
    '\x0',
    '\x0',
    '\xD',
    'I',
    'H',
    'D',
    'R',
    '\x0',
    '\x0',
    '\x0',
    '\x10',
    '\x0',
    '\x0',
    '\x0',
    '\x10',
    '\x8',
    '\x6',
    '\x0',
    '\x0',
    '\x0',
    '\x1F',
    '\xF3',
    '\xFF',
    'a',
    '\x0',
    '\x0',
    '\x0',
    '\x19',
    't',
    'E',
    'X',
    't',
    'S',
    'o',
    'f',
    't',
    'w',
    'a',
    'r',
    'e',
    '\x0',
    'A',
    'd',
    'o',
    'b',
    'e',
    '\x20',
    'I',
    'm',
    'a',
    'g',
    'e',
    'R',
    'e',
    'a',
    'd',
    'y',
    'q',
    '\xC9',
    'e',
    '\x3C',
    '\x0',
    '\x0',
    '\x1',
    '\xCD',
    'I',
    'D',
    'A',
    'T',
    'x',
    '\xDA',
    '\x94',
    '\x93',
    '9',
    'H',
    '\x3',
    'A',
    '\x14',
    '\x86',
    '\xFF',
    '\x5D',
    'b',
    '\xA7',
    '\x4',
    'R',
    '\xC4',
    'm',
    '\x22',
    '\x1E',
    '\xA0',
    'F',
    '\x24',
    '\x8',
    '\x16',
    '\x16',
    'v',
    '\xA',
    '6',
    '\xBA',
    'J',
    '\x9A',
    '\x80',
    '\x8',
    'A',
    '\xB4',
    'q',
    '\x85',
    'X',
    '\x89',
    'G',
    '\xB0',
    'I',
    '\xA9',
    'Q',
    '\x24',
    '\xCD',
    '\xA6',
    '\x8',
    '\xA4',
    'H',
    'c',
    '\x91',
    'B',
    '\xB',
    '\xAF',
    'V',
    '\xC1',
    'F',
    '\xB4',
    '\x15',
    '\xCF',
    '\x22',
    'X',
    '\x98',
    '\xB',
    'T',
    'H',
    '\x8A',
    'd',
    '\x93',
    '\x8D',
    '\xFB',
    'F',
    'g',
    '\xC9',
    '\x1A',
    '\x14',
    '\x7D',
    '\xF0',
    'f',
    'v',
    'f',
    '\xDF',
    '\x7C',
    '\xEF',
    '\xE7',
    'g',
    'F',
    '\xA8',
    '\xD5',
    'j',
    'H',
    '\x24',
    '\x12',
    '\x2A',
    '\x0',
    '\x5',
    '\xBF',
    'G',
    '\xD4',
    '\xEF',
    '\xF7',
    '\x2F',
    '6',
    '\xEC',
    '\x12',
    '\x20',
    '\x1E',
    '\x8F',
    '\xD7',
    '\xAA',
    '\xD5',
    '\xEA',
    '\xAF',
    'I',
    '5',
    'F',
    '\xAA',
    'T',
    '\x5F',
    '\x9F',
    '\x22',
    'A',
    '\x2A',
    '\x95',
    '\xA',
    '\x83',
    '\xE5',
    'r',
    '9',
    'd',
    '\xB3',
    'Y',
    '\x96',
    '\x99',
    'L',
    '\x6',
    '\xE9',
    't',
    '\x9A',
    '\x25',
    '\x85',
    '\x2C',
    '\xCB',
    'T',
    '\xA7',
    '\xC4',
    'b',
    '1',
    '\xB5',
    '\x5E',
    '\x0',
    '\x3',
    'h',
    '\x9A',
    '\xC6',
    '\x16',
    '\x82',
    '\x20',
    'X',
    'R',
    '\x14',
    'E',
    '6',
    'S',
    '\x94',
    '\xCB',
    'e',
    'x',
    '\xBD',
    '\x5E',
    '\xAA',
    'U',
    'T',
    '\x23',
    'L',
    '\xC0',
    '\xE0',
    '\xE2',
    '\xC1',
    '\x8F',
    '\x0',
    '\x9E',
    '\xBC',
    '\x9',
    'A',
    '\x7C',
    '\x3E',
    '\x1F',
    '\x83',
    'D',
    '\x22',
    '\x11',
    '\xD5',
    'T',
    '\x40',
    '\x3F',
    '8',
    '\x80',
    'w',
    '\xE5',
    '3',
    '\x7',
    '\xB8',
    '\x5C',
    '\x2E',
    'H',
    '\x92',
    '\x4',
    '\x87',
    '\xC3',
    '\x81',
    '\x40',
    '\x20',
    '\x40',
    'g',
    '\x98',
    '\xE9',
    '6',
    '\x1A',
    '\xA6',
    'g',
    '\x15',
    '\x4',
    '\xE3',
    '\xD7',
    '\xC8',
    '\xBD',
    '\x15',
    '\xE1',
    'i',
    '\xB7',
    'C',
    '\xAB',
    '\xEA',
    'x',
    '\x2F',
    'j',
    'X',
    '\x92',
    '\xBB',
    '\x18',
    '\x20',
    '\x9F',
    '\xCF',
    '3',
    '\xC3',
    '\xB8',
    '\xE9',
    'N',
    '\xA7',
    '\xD3',
    'l',
    'J',
    '\x0',
    'i',
    '6',
    '\x7C',
    '\x8E',
    '\xE1',
    '\xFE',
    'V',
    '\x84',
    '\xE7',
    '\x3C',
    '\x9F',
    'r',
    '\x2B',
    '\x3A',
    'B',
    '\x7B',
    '7',
    'f',
    'w',
    '\xAE',
    '\x8E',
    '\xE',
    '\xF3',
    '\xBD',
    'R',
    '\xA9',
    'd',
    '\x2',
    'B',
    '\xAF',
    '\x85',
    '2',
    'f',
    'F',
    '\xBA',
    '\xC',
    '\xD9',
    '\x9F',
    '\x1D',
    '\x9A',
    'l',
    '\x22',
    '\xE6',
    '\xC7',
    '\x3A',
    '\x2C',
    '\x80',
    '\xEF',
    '\xC1',
    '\x15',
    '\x90',
    '\x7',
    '\x93',
    '\xA2',
    '\x28',
    '\xA0',
    'S',
    'j',
    '\xB1',
    '\xB8',
    '\xDF',
    '\x29',
    '5',
    'C',
    '\xE',
    '\x3F',
    'X',
    '\xFC',
    '\x98',
    '\xDA',
    'y',
    'j',
    'P',
    '\x40',
    '\x0',
    '\x87',
    '\xAE',
    '\x1B',
    '\x17',
    'B',
    '\xB4',
    '\x3A',
    '\x3F',
    '\xBE',
    'y',
    '\xC7',
    '\xA',
    '\x26',
    '\xB6',
    '\xEE',
    '\xD9',
    '\x9A',
    '\x60',
    '\x14',
    '\x93',
    '\xDB',
    '\x8F',
    '\xD',
    '\xA',
    '\x2E',
    '\xE9',
    '\x23',
    '\x95',
    '\x29',
    'X',
    '\x0',
    '\x27',
    '\xEB',
    'n',
    'V',
    'p',
    '\xBC',
    '\xD6',
    '\xCB',
    '\xD6',
    'G',
    '\xAB',
    '\x3D',
    'l',
    '\x7D',
    '\xB8',
    '\xD2',
    '\xDD',
    '\xA0',
    '\x60',
    '\x83',
    '\xBA',
    '\xEF',
    '\x5F',
    '\xA4',
    '\xEA',
    '\xCC',
    '\x2',
    'N',
    '\xAE',
    '\x5E',
    'p',
    '\x1A',
    '\xEC',
    '\xB3',
    '\x40',
    '9',
    '\xAC',
    '\xFE',
    '\xF2',
    '\x91',
    '\x89',
    'g',
    '\x91',
    '\x85',
    '\x21',
    '\xA8',
    '\x87',
    '\xB7',
    'X',
    '\x7E',
    '\x7E',
    '\x85',
    '\xBB',
    '\xCD',
    'N',
    'N',
    'b',
    't',
    '\x40',
    '\xFA',
    '\x93',
    '\x89',
    '\xEC',
    '\x1E',
    '\xEC',
    '\x86',
    '\x2',
    'H',
    '\x26',
    '\x93',
    '\xD0',
    'u',
    '\x1D',
    '\x7F',
    '\x9',
    '2',
    '\x95',
    '\xBF',
    '\x1F',
    '\xDB',
    '\xD7',
    'c',
    '\x8A',
    '\x1A',
    '\xF7',
    '\x5C',
    '\xC1',
    '\xFF',
    '\x22',
    'J',
    '\xC3',
    '\x87',
    '\x0',
    '\x3',
    '\x0',
    'K',
    '\xBB',
    '\xF8',
    '\xD6',
    '\x2A',
    'v',
    '\x98',
    'I',
    '\x0',
    '\x0',
    '\x0',
    '\x0',
    'I',
    'E',
    'N',
    'D',
    '\xAE',
    'B',
    '\x60',
    '\x82',
};

void MimeType::init()
{
    mime[".html"] = "text/html";
    mime[".avi"] = "video/x-msvideo";
    mime[".bmp"] = "image/bmp";
    mime[".c"] = "text/plain";
    mime[".doc"] = "application/msword";
    mime[".gif"] = "image/gif";
    mime[".gz"] = "application/x-gzip";
    mime[".htm"] = "text/html";
    mime[".ico"] = "image/x-icon";
    mime[".jpg"] = "image/jpeg";
    mime[".png"] = "image/png";
    mime[".txt"] = "text/plain";
    mime[".mp3"] = "audio/mp3";
    mime["default"] = "text/html";
}

std::string MimeType::getMime(const std::string &suffix)
{
    pthread_once(&once_control, MimeType::init);
    if (mime.find(suffix) == mime.end())
        return mime["default"];
    else
        return mime[suffix];
}

HttpData::HttpData(EventLoop *loop, int connectfd)
    : loop_(loop),
      connectfd_(connectfd),
      channel_(new Channel(loop, connectfd)),
      connectionState_(H_CONNECTED),
      error_(false),
      processState_(STATE_PARSE_URI),
      keepAlive_(false),
      nowRead_(0),

      hState_(H_START)
{
    channel_->setReadCallback(std::bind(&HttpData::handleRead, this));
    channel_->setWriteCallback(std::bind(&HttpData::handleWrite, this));
    channel_->setConnCallback(std::bind(&HttpData::handleConn, this));
}

void HttpData::reset()
{
    filename_.clear();
    nowRead_ = 0;
    path_.clear();
    processState_ = STATE_PARSE_URI;

    hState_ = H_START;

    headers_.clear();
    if (timerNode_.lock())
    {
        std::shared_ptr<TimerNode> mytimer(timerNode_.lock());
        mytimer->clearRequest();
        timerNode_.reset();
    }
}

void HttpData::handleRead()
{

    __uint32_t &events = channel_->getEvent();
    do
    {
        int readsum = 0;
        bool zero = false;
        readsum = readn(connectfd_, inBuffer_, zero);
        LOG << "Request :" << inBuffer_;
        if (connectionState_ == H_DISCONNECTING)
        {
            inBuffer_.clear();
            break;
        }
        if (readsum < 0)
        {
            LOG << "READ ERROR";
            error_ = true;
            handleError(connectfd_, 400, "Bad Request");
            break;
        }
        else if (zero)
        {
            // 有请求但读不到 一律当对端已经关闭处理
            connectionState_ = H_DISCONNECTING;
            if (readsum == 0)
            {
                break;
            }
        }
        if (processState_ == STATE_PARSE_URI)
        {
            URIState flag = this->parseURI();
            if (flag == PARSE_URI_AGAIN)
                break;
            else if (flag == PARSE_URI_ERROR)
            {
                LOG << "[ERROR] parse uri error";
                inBuffer_.clear();
                error_ = true;
                handleError(connectfd_, 400, "Bad Request");
                break;
            }
            else
            {
                processState_ = STATE_PARSE_HEADERS;
            }
        }
        if (processState_ == STATE_PARSE_HEADERS)
        {
            HeaderState flag = this->parseHeaders();
            if (flag == PARSE_HEADER_AGAIN)
                break;
            else if (flag == PARSE_HEADER_ERROR)
            {
                LOG << "[ERROR] parse header error";
                error_ = true;
                handleError(connectfd_, 400, "Bad Request");
                break;
            }
            if (method_ == POST)
            {
                processState_ = STATE_PARSE_BODY;
            }
            else
            {
                processState_ = STATE_ANALYSIS;
            }
        }
        if (processState_ == STATE_PARSE_BODY)
        {
            int content_length = -1;
            if (headers_.find("Content-length") != headers_.end())
            {
                content_length = stoi(headers_["Content-length"]);
            }
            else
            {
                LOG << "[ERROR] parse body error";
                error_ = true;
                handleError(connectfd_, 400, "Bad Request: Lack of argument (Content-length)");
                break;
            }
            if (static_cast<int>(inBuffer_.size()) < content_length)
                break;
            processState_ = STATE_ANALYSIS;
        }
        if (processState_ == STATE_ANALYSIS)
        {
            AnalysisState flag = this->analysisRequest();
            if (flag == ANALYSIS_SUCCESS)
            {
                processState_ = STATE_FINISH;
                break;
            }
            else
            {
                LOG << "[ERROR] analysis error";
                error_ = true;
                break;
            }
        }
    } while (false);

    if (!error_)
    {
        if (outBuffer_.size() > 0)
        {
            handleWrite();
        }
        if (!error_ && processState_ == STATE_FINISH)
        {
            this->reset();
            if (inBuffer_.size() > 0)
            {
                //  ?
                if (connectionState_ != H_DISCONNECTING)
                    handleRead();
            }
        }
        else if (!error_ && connectionState_ != H_DISCONNECTED)
            events |= EPOLLIN;
    }
}

void HttpData::handleWrite()
{

    if (!error_ && H_DISCONNECTED != connectionState_)
    {
        __uint32_t &event = channel_->getEvent();
        if (writen(connectfd_, outBuffer_) < 0)
        {
            error_ = true;
            event = 0;
            LOG << "[ERROR] write error";
        }
        if (outBuffer_.size() > 0)
        {
            event |= EPOLLOUT;
        }
    }
}

void HttpData::handleConn()
{
    // std::cout << "http conn" << std::endl;
    seperateTimer();
    // 主要负责处理连接超时的问题
    __uint32_t &event = channel_->getEvent();
    if (!error_ && connectionState_ == H_CONNECTED)
    {

        if (event != 0)
        {
            int timeout = DEFAULT_EXPIRED_TIME;
            if (keepAlive_)
            {
                timeout = DEFAULT_KEEP_ALIVE_TIME;
            }
            if ((event & EPOLLIN) && (event & EPOLLOUT))
            {
                event = __uint32_t(0);
                event |= EPOLLOUT;
            }
            event |= EPOLLET;
            loop_->updatePoller(channel_, timeout);
        }
        else if (keepAlive_)
        {
            event |= (EPOLLET | EPOLLIN);
            int timeout = DEFAULT_KEEP_ALIVE_TIME;
        }
        else
        {
            event |= (EPOLLIN | EPOLLET);

            int timeout = (DEFAULT_KEEP_ALIVE_TIME >> 1);
            loop_->updatePoller(channel_, timeout);
        }
    }
    else if (!error_ && connectionState_ == H_DISCONNECTING &&
             (event & EPOLLOUT))
    {
        event |= (EPOLLIN | EPOLLET);
    }
    else
    {
        loop_->runInLoop(std::bind(&HttpData::handleClose, shared_from_this()));
    }
}

void HttpData::handleError(int fd, int stateCode, std::string msg)
{
    // 写死的错误报文
    msg = " " + msg;
    char send_buf[4096];
    std::string header_buf, body_buf;
    body_buf += "<html><title>哎~出错了</title>";
    body_buf += "<body bgcolor=\"ffffff\">";
    body_buf += std::to_string(stateCode) + msg;
    body_buf += "<hr><em> jiajun's Web Server</em>\n</body></html>";

    header_buf += "HTTP/1.1 " + std::to_string(stateCode) + msg + "\r\n";
    header_buf += "Content-Type: text/html\r\n";
    header_buf += "Connection: Close\r\n";
    header_buf += "Content-Length: " + std::to_string(body_buf.size()) + "\r\n";
    header_buf += "Server: jiajun's web server\r\n";
    header_buf += "\r\n";

    sprintf(send_buf, "%s", header_buf.c_str());
    writen(fd, send_buf, strlen(send_buf));
    sprintf(send_buf, "%s", body_buf.c_str());
    writen(fd, send_buf, strlen(send_buf));
}

void HttpData::handleClose()
{
    connectionState_ = H_DISCONNECTED;
    std::shared_ptr<HttpData> guard(shared_from_this());
    loop_->removeFromPoller(channel_);
}
void HttpData::seperateTimer()
{
    // 释放定时器
    if (timerNode_.lock())
    {
        std::shared_ptr<TimerNode> mytimer(timerNode_.lock());
        mytimer->clearRequest();
        timerNode_.reset();
    }
}
URIState HttpData::parseURI()
{
    std::string &str = inBuffer_;
    std::string cop = str;
    size_t pos = str.find('\r', nowRead_);
    if (pos < 0)
    {
        return PARSE_URI_AGAIN;
    }
    std::string request_line = str.substr(0, pos);
    // 将剩下数据去掉请求行
    if (str.size() > pos + 1)
        str = str.substr(pos + 1);
    else
        str.clear();
    int get = request_line.find("GET");
    int post = request_line.find("POST");
    int head = request_line.find("HEAD");
    if (get >= 0)
    {
        pos = get;
        method_ = GET;
    }
    else if (post >= 0)
    {
        pos = post;
        method_ = POST;
    }
    else if (head >= 0)
    {
        pos = head;
        method_ = HEAD;
    }
    else
    {
        return PARSE_URI_ERROR;
    }
    pos = request_line.find("/", pos);
    if (pos < 0)
    {
        filename_ = "index.html";
        httpVersion_ = HTTP_11;
        return PARSE_URI_SUCCESS;
    }
    else
    {
        size_t _pos = request_line.find(' ', pos);
        if (_pos < 0)
            return PARSE_URI_ERROR;
        else
        {
            if (_pos - pos > 1)
            {
                filename_ = request_line.substr(pos + 1, _pos - pos - 1);
                size_t __pos = filename_.find('?');
                if (__pos >= 0)
                {
                    filename_ = filename_.substr(0, __pos);
                }
            }

            else
                filename_ = "index.html";
        }
        pos = _pos;
    }

    // HTTP 版本号
    pos = request_line.find("/", pos);
    if (pos < 0)
        return PARSE_URI_ERROR;
    else
    {
        if (request_line.size() - pos <= 3)
            return PARSE_URI_ERROR;
        else
        {
            std::string ver = request_line.substr(pos + 1, 3);
            if (ver == "1.0")
                httpVersion_ = HTTP_10;
            else if (ver == "1.1")
                httpVersion_ = HTTP_11;
            else
                return PARSE_URI_ERROR;
        }
    }
    return PARSE_URI_SUCCESS;
}
HeaderState HttpData::parseHeaders()
{
    std::string &str = inBuffer_;
    int key_start = -1, key_end = -1, value_start = -1, value_end = -1;
    int now_read_line_begin = 0;
    bool notFinish = true;
    size_t i = 0;
    for (; i < str.size() && notFinish; ++i)
    {
        switch (hState_)
        {
        case H_START:
        {
            if (str[i] == '\n' || str[i] == '\r')
                break;
            hState_ = H_KEY;
            key_start = i;
            now_read_line_begin = i;
            break;
        }
        case H_KEY:
        {
            if (str[i] == ':')
            {
                key_end = i;
                if (key_end - key_start <= 0)
                    return PARSE_HEADER_ERROR;
                hState_ = H_COLON;
            }
            else if (str[i] == '\n' || str[i] == '\r')
                return PARSE_HEADER_ERROR;
            break;
        }
        case H_COLON:
        {
            if (str[i] == ' ')
            {
                hState_ = H_SPACES_AFTER_COLON;
            }
            else
                return PARSE_HEADER_ERROR;
            break;
        }
        case H_SPACES_AFTER_COLON:
        {
            hState_ = H_VALUE;
            value_start = i;
            break;
        }
        case H_VALUE:
        {
            if (str[i] == '\r')
            {
                hState_ = H_CR;
                value_end = i;
                if (value_end - value_start <= 0)
                    return PARSE_HEADER_ERROR;
            }
            else if (i - value_start > 255)
                return PARSE_HEADER_ERROR;
            break;
        }
        case H_CR:
        {
            if (str[i] == '\n')
            {
                hState_ = H_LF;
                std::string key(str.begin() + key_start, str.begin() + key_end);
                std::string value(str.begin() + value_start, str.begin() + value_end);
                headers_[key] = value;
                now_read_line_begin = i;
            }
            else
                return PARSE_HEADER_ERROR;
            break;
        }
        case H_LF:
        {
            if (str[i] == '\r')
            {
                hState_ = H_END_CR;
            }
            else
            {
                key_start = i;
                hState_ = H_KEY;
            }
            break;
        }
        case H_END_CR:
        {
            if (str[i] == '\n')
            {
                hState_ = H_END_LF;
            }
            else
                return PARSE_HEADER_ERROR;
            break;
        }
        case H_END_LF:
        {
            notFinish = false;
            key_start = i;
            now_read_line_begin = i;
            break;
        }
        }
    }
    if (hState_ == H_END_LF)
    {
        str = str.substr(i);
        return PARSE_HEADER_SUCCESS;
    }
    str = str.substr(now_read_line_begin);
    return PARSE_HEADER_AGAIN;
}
AnalysisState HttpData::analysisRequest()
{
    if (method_ == POST)
    {
    }
    else if (method_ == GET || method_ == HEAD)
    {
        std::string header;
        header += "HTTP/1.1 200 OK\r\n";
        if (headers_.find("Connection") != headers_.end() &&
            (headers_["Connection"] == "Keep-Alive" ||
             headers_["Connection"] == "keep-alive"))
        {
            keepAlive_ = true;
            header += std::string("Connection: Keep-Alive\r\n") + "Keep-Alive: timeout=" +
                      std::to_string(DEFAULT_KEEP_ALIVE_TIME) + "\r\n";
        }
        int dot_pos = filename_.find('.');
        std::string filetype;
        if (dot_pos < 0)
            filetype = MimeType::getMime("default");
        else
            filetype = MimeType::getMime(filename_.substr(dot_pos));

        // echo test
        if (filename_ == "hello")
        {
            outBuffer_ =
                "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\nHello World";
            return ANALYSIS_SUCCESS;
        }
        if (filename_ == "favicon.ico")
        {
            header += "Content-Type: image/png\r\n";
            header += "Content-Length: " + std::to_string(sizeof favicon) + "\r\n";
            header += "Server: LinYa's Web Server\r\n";

            header += "\r\n";
            outBuffer_ += header;
            outBuffer_ += std::string(favicon, favicon + sizeof favicon);
            ;
            return ANALYSIS_SUCCESS;
        }

        struct stat sbuf;
        if (stat(filename_.c_str(), &sbuf) < 0)
        {
            header.clear();
            handleError(connectfd_, 404, "Not Found!");
            return ANALYSIS_ERROR;
        }
        header += "Content-Type: " + filetype + "\r\n";
        header += "Content-Length: " + std::to_string(sbuf.st_size) + "\r\n";
        header += "Server: LinYa's Web Server\r\n";
        // 头部结束
        header += "\r\n";
        outBuffer_ += header;

        if (method_ == HEAD)
            return ANALYSIS_SUCCESS;

        int src_fd = open(filename_.c_str(), O_RDONLY, 0);
        if (src_fd < 0)
        {
            outBuffer_.clear();
            handleError(connectfd_, 404, "Not Found!");
            return ANALYSIS_ERROR;
        }
        void *mmapRet = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
        close(src_fd);
        if (mmapRet == (void *)-1)
        {
            munmap(mmapRet, sbuf.st_size);
            outBuffer_.clear();
            handleError(connectfd_, 404, "Not Found!");
            return ANALYSIS_ERROR;
        }
        char *src_addr = static_cast<char *>(mmapRet);
        outBuffer_ += std::string(src_addr, src_addr + sbuf.st_size);
        ;
        munmap(mmapRet, sbuf.st_size);
        return ANALYSIS_SUCCESS;
    }
    return ANALYSIS_ERROR;
}

void HttpData::newEvent()
{
    channel_->setEvent(DEFAULT_EVENT);
    loop_->addToPoller(channel_, DEFAULT_EXPIRED_TIME);
}