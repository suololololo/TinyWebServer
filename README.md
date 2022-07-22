<!--
 * @Author: jiajun
 * @Date: 2022-07-22 16:28:58
 * @FilePath: /TinyWebServer/README.md
-->
<h1> TinyWebServer </h1> 
基于reactor模式 服务器的C++实现

**特点:**
1. 基于reactor模式，非阻塞IO,EPOLL+EL的IO多路复用技术
2. 基于小根堆实现的定时器关闭超时请求
3. 借鉴muduo网络库对server的设计，采用one loop per thread + IO multiplexing设计模式。
4. 日志设计采用双缓冲技术设计，避免了频繁IO将日志记录写入文件中。 
5. 使用RAII机制封装锁变量，使用智能指针有效减少内存泄漏
6. 主线程只负责accept连接，后将事件以轮询的方式分发给其他工作线程，锁的争用只出现在主线程和某一工作线程中。
7. 采用线程池避免了线程频繁创建和销毁的性能开销。
8. 状态机解析HTTP请求
9. 使用eventfd异步唤醒线程

<h2>Build</h2>
```
./build.sh
```

<h2>Usage</h2>
```
./main [-t thread_num] [-p port]  [-l log_file_path(should begin with '/')]
```

qps 短链接qps 26875

