#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/errno.h>

#include "wrap.h"
#include "sysdata.h"

#define MAXLINE 80
#define SERV_PORT 8000
#define SERV_IP "127.0.0.1"
#define CLI_NUM 10000    //

//#ifdef __cplusplus
//extern "C"
//{
//#endif

static int sockfd_array[CLI_NUM] = {0};
static char buf[MAXLINE] = {0};

void epoll_init()
{
    struct sockaddr_in servaddr;

    int sockfd = 0;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERV_IP, &servaddr.sin_addr);
    servaddr.sin_port = htons(SERV_PORT);

    int i = 0;
    for (i = 0; i < CLI_NUM; i++)
    {
        sockfd = Socket(AF_INET, SOCK_STREAM, 0);
        Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

        sockfd_array[i] =  sockfd;
        printf("i = %d\n", i);

        //rceived from server(necessary)
        char rece_ser[10] = {0};
        Read(sockfd, rece_ser, 10);
        printf("received = %s\n", rece_ser);

    }
    printf("--------\n");
}





void sysdate_to_buf(int k)
{
    SysData sysData;

    if (k == 0)
    {
        char buf_temp[10] = {0};
        getMemory(&sysData);
        getCpuRate(&sysData);   //获取CPU使用率需要sleep(1)

        sprintf(buf_temp, "%d", sysData.m_cpuRate);
        int j = 0;
        for (int i = 0; buf_temp[i] != '\0'; i++)
        {
            buf[j++] = buf_temp[i];
        }
        buf[j++] = 'R';

        sprintf(buf_temp, "%d", sysData.m_memoryUse);
        for (int i = 0; buf_temp[i] != '\0'; i++)
        {
            buf[j++] = buf_temp[i];
        }
        buf[j++] = 'U';

        sprintf(buf_temp, "%d", sysData.m_memoryTotal);
        for (int i = 0; buf_temp[i] != '\0'; i++)
        {
            buf[j++] = buf_temp[i];
        }
        buf[j++] = 'T';
    }
    else
    {
        char buf_temp[10] = {0};

        sprintf(buf_temp, "%d", 35 + (k%50));
        int j = 0;
        for (int i = 0; buf_temp[i] != '\0'; i++)
        {
            buf[j++] = buf_temp[i];
        }
        buf[j++] = 'R';

        sprintf(buf_temp, "%d", (4000 + (k%3000)));
        for (int i = 0; buf_temp[i] != '\0'; i++)
        {
            buf[j++] = buf_temp[i];
        }
        buf[j++] = 'U';

        sprintf(buf_temp, "%d", 8000);
        for (int i = 0; buf_temp[i] != '\0'; i++)
        {
            buf[j++] = buf_temp[i];
        }
        buf[j++] = 'T';
    }

}


void epoll_while()
{

    int i = 0;
    while (1)
    {
        for (i = 0; i < CLI_NUM; i++)
        {
            memset(buf, 0, MAXLINE);
            sysdate_to_buf(i);
            Write(sockfd_array[i], buf, strlen(buf));
        }
        printf("sleep!\n");
        sleep(1);
    }
}


void epoll_close()
{
    int i = 0;
    //close sockfd
    for (i = 0; i < CLI_NUM; i++)
    {
        close(sockfd_array[i]);
    }
}

//#ifdef __cplusplus
//}
//#endif
