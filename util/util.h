/*
 * @Author: jiajun
 * @Date: 2022-07-17 15:19:20
 * @FilePath: /TinyWebServer/util/util.h
 */
#ifndef __UTIL_H__
#define __UTIL_H__
#include <sys/types.h>
#include <string>
ssize_t writen(int fd, void *buf, size_t n);
int socket_bind_listen(int port);
int setNonBlocking(int fd);
void setSoketNondelay(int fd);
ssize_t readn(int fd, std::string &buf, bool &zero);
ssize_t readn(int fd, void *buff, size_t n);
ssize_t writen(int fd, std::string &sbuff);
void handle_for_sigpipe();
#endif