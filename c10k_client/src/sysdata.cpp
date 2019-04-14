#include "sysdata.h"

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

//    sysinfo(&info);
//    printf("memory total: %d M\n", (int)(info.totalram/1024/1024));
//    printf("memory use  : %d M\n", (long)(info.totalram-info.freeram)/1024/1024);
//    printf("memory free : %d M\n", (int)(info.freeram/1024/1024));

}
