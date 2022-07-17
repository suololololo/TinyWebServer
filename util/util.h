#ifndef __UTIL_H__
#define __UTIL_H__
#include <sys/types.h>
ssize_t writen(int fd, void *buf, size_t n);
int socket_bind_listen(int port);
#endif