/*
 * @Author: jiajun
 * @Date: 2022-07-16 16:54:38
 * @FilePath: /TinyWebServer/main.cc
 */
#include "server/server.h"
#include "server/eventloop.h"
#include "log/logging.h"
#include <getopt.h>
#include <iostream>
using std::string;


int main(int argc, char *argv[])
{
    int threadNum = 4;
    int port = 3456;
    string logPath = "/home/jiajun/TinyWebServer/webserver.log";

    // parse args
    int opt;
    const char *str = "t:l:p:";
    while ((opt = getopt(argc, argv, str)) != -1)
    {
        switch (opt)
        {
        case 't':
        {
            threadNum = atoi(optarg);
            break;
        }
        case 'l':
        {
            logPath = optarg;
            if (logPath.size() < 2 || optarg[0] != '/')
            {
                printf("logPath should start with \"/\"\n");
                abort();
            }
            break;
        }
        case 'p':
        {
            port = atoi(optarg);
            break;
        }
        default:
            break;
        }
    }
    Logger::set_log_filename(logPath.c_str());

    EventLoop baseloop;
    Server server(&baseloop, threadNum, port);
    server.start();
    baseloop.loop();
    std::cout << "stop " << std::endl;
    return 0;
}