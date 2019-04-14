#ifndef EPOLL_SER_H
#define EPOLL_SER_H

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


//把客户端的socket设置为非阻塞方式
void setnonblocking(int sock);
//改变connfd读写属性
void update_events(int epfd, int connfd, int events, int op);
void epoll_init();
int epoll_nfds();
void epoll_close();
void handleAccept(int epfd, struct epoll_event *evs);
void handleRead(int epfd, struct epoll_event *evs);
void handleWrite(int epfd, struct epoll_event *events);


#endif // EPOLL_SER_H
