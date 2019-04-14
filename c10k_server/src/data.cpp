#include "data.h"

unsigned long GetCPUTotalTime(CPUInfo* pstCPUInfo)
{
    return pstCPUInfo->user + pstCPUInfo->nice + pstCPUInfo->system +
        pstCPUInfo->idle + pstCPUInfo->iowait + pstCPUInfo->irq + pstCPUInfo->softirq;
}

void DisplayCPUInfo(CPUInfo* pstCPUInfo)
{
    printf("%s %lu %lu %lu %lu %lu %lu %lu\n", pstCPUInfo->name, pstCPUInfo->user, pstCPUInfo->nice, pstCPUInfo->system,
        pstCPUInfo->idle, pstCPUInfo->iowait, pstCPUInfo->irq, pstCPUInfo->softirq);
}

int GetCPUInfo(CPUInfo *pstCPUInfo)
{
    FILE * fp = fopen("/proc/stat", "r");

    char buf[1024];
    fgets(buf, sizeof(buf), fp);

    sscanf(buf,"%s %lu %lu %lu %lu %lu %lu %lu",
        pstCPUInfo->name, &pstCPUInfo->user, &pstCPUInfo->nice,&pstCPUInfo->system,
        &pstCPUInfo->idle, &pstCPUInfo->iowait, &pstCPUInfo->irq,&pstCPUInfo->softirq);

    fclose(fp);

    return 0;
}

unsigned long GetCPUUsageRate()
{
    CPUInfo stCPUInfo1;
    CPUInfo stCPUInfo2;

    memset(&stCPUInfo1, 0, sizeof(stCPUInfo1));
    memset(&stCPUInfo2, 0, sizeof(stCPUInfo2));

    GetCPUInfo(&stCPUInfo1);
    //DisplayCPUInfo(&stCPUInfo1);
    sleep(1);
    GetCPUInfo(&stCPUInfo2);
    //DisplayCPUInfo(&stCPUInfo2);

    unsigned long nTotalTime = GetCPUTotalTime(&stCPUInfo2) - GetCPUTotalTime(&stCPUInfo1);
    unsigned long nIdleTime = stCPUInfo2.idle - stCPUInfo1.idle;
    printf("total time: %lu\n", nTotalTime);
    printf("idle time:  %lu\n", nIdleTime);	//等待时间
    if (nTotalTime == 0)
    {
        return 0;
    }

    unsigned long usage = (nTotalTime - nIdleTime) * 100 / nTotalTime;

    return usage;

}

void getCpuRate(SysData *sysData)
{
    sysData->m_cpuRate =  GetCPUUsageRate();
}


void getMemory(SysData *sysData)
{
    //获取内存信息
    struct sysinfo info;
    sysinfo(&info);
    sysData->m_memoryTotal = (int)(info.totalram/1024/1024);
    sysData->m_memoryUse = (int)((info.totalram-info.freeram)/1024/1024);
}

int sysdate_to_buf(char *buf, SysData *sysData)
{
    char buf_temp[10] = {0};

    sprintf(buf_temp, "%d", sysData->m_cpuRate);
    int j = 0;
    for (int i = 0; buf_temp[i] != '\0'; i++)
    {
        buf[j++] = buf_temp[i];
    }
    buf[j++] = 'R';

    sprintf(buf_temp, "%d", sysData->m_memoryUse);
    for (int i = 0; buf_temp[i] != '\0'; i++)
    {
        buf[j++] = buf_temp[i];
    }
    buf[j++] = 'U';

    sprintf(buf_temp, "%d", sysData->m_memoryTotal);
    for (int i = 0; buf_temp[i] != '\0'; i++)
    {
        buf[j++] = buf_temp[i];
    }
    buf[j++] = 'T';

    return j; //return deal char number
}

void sysdate_to_bufff()
{

}

void deCode(const char *str, SysData *sysData)
{
    int temp = 0;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            temp = temp * 10 + str[i] - '0';

        }
        else if (str[i] == 'R')
        {
            sysData->m_cpuRate = temp;
            temp = 0;
        }
        else if (str[i] == 'U')
        {
            sysData->m_memoryUse = temp;
            temp = 0;
        }
        else if (str[i] == 'T')
        {
            sysData->m_memoryTotal = temp;
            temp = 0;
            break;
        }
    }
}

