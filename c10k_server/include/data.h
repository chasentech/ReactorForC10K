#ifndef DATA_H
#define DATA_H


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdlib.h>

typedef struct cpu_info
{
    char name[8];
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;	//等待时间
    unsigned long iowait;
    unsigned long irq;
    unsigned long softirq;
} CPUInfo;

typedef struct CliData
{
    int fd;  //文件描述符
    char IP[16];  //登录IP地址
    int port;  //登录的端口号
} CliData;


typedef struct SysData
{
    int m_cpuRate;  //0-100
    int m_memoryUse;    //
    int m_memoryTotal;  //
} SysData;


typedef struct SaveData
{
    CliData clidata;
    SysData sysData;
} SaveData;

void getCpuRate(SysData *sysData);
void getMemory(SysData *sysData);

int sysdate_to_buf(char *buf, SysData *sysData);
void deCode(const char *str, SysData *sysData);

#endif // DATA_H
