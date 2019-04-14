#include "epoll_ser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <queue>
#include <map>

#include "data.h"
#include "threadpool.h"
#include "epoll_ser.h"
#include "wrap.h"

using namespace std;


#define MAXLINE 100

#define SERV_PORT 8000
#define CLI_NUM 10000
#define RETURN_MAG "Already return!\n"

//接收消息缓冲区
char buff[MAXLINE] = {0};

//epoll_event结构体, ev用于注册事件, events数组用于回传要处理的事件
struct epoll_event ev, events[CLI_NUM];

//生成用于处理epoll专用的文件描述符, 参数256可随意，现在该参数的作用被遗弃。
int epfd = epoll_create(CLI_NUM + 1);		//CLI_NUM + 1

//创建套接字
int listenfd = Socket(AF_INET, SOCK_STREAM, 0);


queue<struct epoll_event> que_evs;//相应epoll事件队列
queue<string> que_data;//读取数据队列
queue<int> que_data_fd;//数据队列对应的fd队列


map<int, SaveData> serSaveData;//服务器保存数据



extern pthread_mutex_t counter_mutex_map;


int monitor_fd = 0; //监视器fd

//把客户端的socket设置为非阻塞方式
void setnonblocking(int sock)
{
    int opts;
    opts = fcntl(sock, F_GETFL);
    if(opts < 0)
    {
        perror("fcntl(sock, GETFL)");
        exit(1);
    }

    opts = opts | O_NONBLOCK;
    if(fcntl(sock, F_SETFL, opts) < 0)
    {
        perror("fcntl(sock,SETFL,opts)");
        exit(1);
    }
}

//改变connfd读写属性
void update_events(int epfd, int connfd, int events, int op)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = events;
    ev.data.fd = connfd;

    int r = epoll_ctl(epfd, op, connfd, &ev);
    if (r != 0)
        perror("epoll_ctl failed");
}


void epoll_init()
{
    //准备服务器地址
    struct sockaddr_in serveraddr;
    serveraddr.sin_family       = AF_INET;
    serveraddr.sin_addr.s_addr  = htonl(INADDR_ANY);
    serveraddr.sin_port         = htons(SERV_PORT);

    //设置socket重用
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //绑定
    Bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    //监听
    Listen(listenfd, CLI_NUM);

    ev.data.fd = listenfd;          //设置epoll管理的socket，
    ev.events = EPOLLIN | EPOLLET;  //设置事件类型为可读事件，工作方式为边沿触发
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);    //注册epoll事件
}

int epoll_nfds()
{
    int nfds = epoll_wait(epfd, events, CLI_NUM, 10); //100ms等待epoll事件的发生
    return nfds;
}

void epoll_close()
{
    close(epfd);
    close(listenfd);
}

void to_return_data(char *p, int fd)
{
    p[0] = 'O';
    p[1] = 'K';
    p[2] = ':';

    int i = 0;
    int k = 10000;
    for (i = 0; k != 0 && i < 10; i++)
    {
        p[i + 3] = fd / k % 10 + '0';
        k /= 10;
    }

}

void handleAccept(int epfd, struct epoll_event *evs)
{
    if(evs->data.fd == listenfd)
    {
        char return_data[10] = {0};

        struct sockaddr_in clientaddr;
        socklen_t clientaddr_len = sizeof(clientaddr);

        int connfd = Accept(listenfd, (struct sockaddr*)&clientaddr, &clientaddr_len);
        if(connfd < 0)
        {
            perror("connfd < 0");
            exit(1);
        }
        setnonblocking(connfd);    //把客户端的socket设置为非阻塞方式

        const char *cli_ip = inet_ntoa(clientaddr.sin_addr);
        int cli_port = ntohs(clientaddr.sin_port);
        //printf("connect from IP: %s, port: %d\n", cli_ip, cli_port);


        //全局表写入
        pthread_mutex_lock(&counter_mutex_map);
        CliData cliData;
        cliData.fd = connfd;
        strcpy(cliData.IP, cli_ip);
        cliData.port = cli_port;
        pthread_mutex_unlock(&counter_mutex_map);


//        serSaveData.insert();
        serSaveData[cliData.fd].clidata = cliData;
        cout << "CliData: " << cliData.fd << " " << cliData.IP << " " << cliData.port << endl;




        //设置事件类型为可读事件，边沿触发，并注册epoll事件
        update_events(epfd, connfd, EPOLLIN | EPOLLET, EPOLL_CTL_ADD);

        //retrun to client(necessary!!!)
        to_return_data(return_data, connfd);
        write(connfd, return_data, 10);
    }
}

void handleRead(int epfd, struct epoll_event *evs)
{
    if (evs->data.fd != listenfd && evs->events & (EPOLLIN | EPOLLERR | EPOLLRDHUP))
    {
        int sockfd = evs->data.fd;
        if (sockfd < 0)
            return;

        if (evs->events & EPOLLRDHUP)
        {
            if (sockfd == monitor_fd) monitor_fd = 0;
            close(sockfd);
            events->data.fd = -1;
            printf("client is EPOLLRDHUP!\n");
        }
        int n = 0;
        memset(buff, 0, MAXLINE);
        if ( (n = read(sockfd, buff, MAXLINE)) < 0)	//连接错误
        {
            if (sockfd == monitor_fd) monitor_fd = 0;
            close(sockfd);
            events->data.fd = -1;
            printf("readsize = %d\n", n);
            printf("readline error\n");
        }
        else if (n == 0)	//断开连接
        {
            if (sockfd == monitor_fd) monitor_fd = 0;
            close(sockfd);
            events->data.fd = -1;
            printf("client close the socket!\n");
        }
        else
        {
            //Monitor cli
            if (buff[0] == 'M')
            {
                buff[0] = 0;
                printf("Monitor is connect!\n");
                write(sockfd, "M", 1);
                monitor_fd = sockfd;
            }
            else    //normal cli
            {
                printf("recv data: %s, fd = %d\n", buff, sockfd);
                que_data.push(buff);
                que_data_fd.push(sockfd);
            }





            //设置事件类型为可写事件，边沿触发，并注册epoll事件
            //update_events(epfd, sockfd, EPOLLOUT | EPOLLET, EPOLL_CTL_MOD);
        }

    }
}

void handleWrite(int epfd, struct epoll_event *events)
{
    int sockfd = events->data.fd;
    write(sockfd, RETURN_MAG, strlen(RETURN_MAG));

    //设置事件类型为可读事件，边沿触发，并注册epoll事件
    //update_events(epfd, sockfd, EPOLLIN | EPOLLET, EPOLL_CTL_MOD);
}

