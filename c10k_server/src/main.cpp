#include <iostream>
#include <queue>
#include <vector>
#include <map>
#include <iomanip>

//#include "wrap.h" //前置声明会出现错误 error: ‘socklen_t’ has not been declared
#include "data.h"
#include "threadpool.h"
#include "epoll_ser.h"
#include "wrap.h"

using namespace std;


//g++ main.cpp epoll_ser.cpp data.cpp threadpool.cpp wrap.cpp -lpthread


//MySql operation
//mysql -p -u root
//grant all privileges on *.* to 'root'@'localhost';
//create user 'c10k'@'localhost' identified by 'ubuntu';
//create database clidatas;
//grant all privileges on clidatas.* to 'c10k'@'localhost';

//mysql -h localhost -p -u c10k
//show databases;
//use clidatas;
//show tables;
//quit



char buf_to_qt[40] = {0};
int w_cpuScale = 0;
int w_memScale = 0;

extern struct epoll_event events[];
extern int epfd;		//CLI_NUM + 1

extern queue<struct epoll_event> que_evs;
extern queue<string> que_data;
extern queue<int> que_data_fd;

extern map<int, SaveData> serSaveData;



extern int monitor_fd;


//dealDate线程池各程序之间的互斥锁
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
//全局表访问互斥锁
pthread_mutex_t counter_mutex_map = PTHREAD_MUTEX_INITIALIZER;

void *pthread_subReactor(void *arg)
{
    cout << (char *)arg << " " << endl;

    while (1)
    {
        if (!que_evs.empty())
        {
            //cout << "que_fds size: " << que_evs.size() << ", fd=" << que_evs.front().data.fd << endl;

            handleAccept(epfd, &que_evs.front());
            handleRead(epfd, &que_evs.front());

            que_evs.pop();
        }
        else usleep(10);
    }
}

void *pthread_monitor(void *arg)
{
    cout << (char *)arg << " " << endl;

    SysData sysDate;
    while (1)
    {
        if (monitor_fd != 0)
        {


            getCpuRate(&sysDate);
            getMemory(&sysDate);

            memset(buf_to_qt, 0, 40);
            int k = 0;

            k += sysdate_to_buf(buf_to_qt, &sysDate);
            k += sysdate_to_buf(buf_to_qt + k, &serSaveData[6].sysData);

            pthread_mutex_lock(&counter_mutex_map);
            buf_to_qt[k++] = w_cpuScale / 100 + '0';
            buf_to_qt[k++] = w_cpuScale / 10 % 10 + '0';
            buf_to_qt[k++] = w_cpuScale % 10 + '0';
            buf_to_qt[k++] = 'E';

            buf_to_qt[k++] = w_memScale / 100 % 10 + '0';
            buf_to_qt[k++] = w_memScale / 10 % 10 + '0';
            buf_to_qt[k++] = w_memScale % 10 + '0';
            buf_to_qt[k++] = 'D';
            pthread_mutex_unlock(&counter_mutex_map);



            //cout << "......." << buf_to_qt << endl;


            write(monitor_fd, buf_to_qt, strlen(buf_to_qt));

            printf("%d, %d, %d\n", sysDate.m_cpuRate, sysDate.m_memoryUse, sysDate.m_memoryTotal);
            //sleep(1); //获取CPU使用率需要sleep(1)
        }
        else sleep(1);

        //全局表读取
        pthread_mutex_lock(&counter_mutex_map);
        int i = 0;
        w_cpuScale = 0;
        w_memScale = 0;
        for(map<int, SaveData>::iterator iter = serSaveData.begin();
            iter != serSaveData.end(); iter++, i++)
        {
           int key = iter->first;
           SaveData value = iter->second;

           if (value.sysData.m_cpuRate > 70) w_cpuScale++;
           if (value.sysData.m_memoryUse > 6000) w_memScale++;

           cout.setf(ios::left);
           cout << setw(6) << key << " " << setw(15) << value.clidata.IP << " " << setw(5) << value.clidata.port << " "
                << setw(3) << value.sysData.m_cpuRate << " " << setw(4) << value.sysData.m_memoryUse << " "
                << setw(4) << value.sysData.m_memoryTotal << endl;
        }
        if (i != 0)
        {
            w_cpuScale = w_cpuScale*100 / i;
            w_memScale = w_memScale*100 / i;
        }
        pthread_mutex_unlock(&counter_mutex_map);

    }
}

void *pthread_dealData(void *arg)
{
    cout << (char *)arg << " " << endl;

    while (1)
    {
        int deal_falg = false;
        string str;
        int srt_fd;

        pthread_mutex_lock(&counter_mutex);
        if (!que_data.empty() && !que_data_fd.empty())
        {
            //cout << "que_data: " << que_data.front() << endl;


            str = que_data.front();
            que_data.pop();

            srt_fd =  que_data_fd.front();
            que_data_fd.pop();

            deal_falg = true;

        }
        else usleep(10);
        pthread_mutex_unlock(&counter_mutex);



        if (deal_falg == true)
        {
            SysData sysData;
            const char *p = str.data();
            deCode(p, &sysData);

//            error!
//            cout << "R: " << sysData.m_cpuRate << " U: " << sysData.m_memoryUse
//                 << " T: " << sysData.m_memoryTotal << endl;

            //全局表写入
            pthread_mutex_lock(&counter_mutex_map);
            serSaveData[srt_fd].sysData = sysData;
            pthread_mutex_unlock(&counter_mutex_map);

            //cout << "fd is " << srt_fd << "str is " << str << endl;
        }
    }
}

int main()
{
    int nfds = 0;

    epoll_init();

    pthread_t ntid_subReactor;
    pthread_t ntid_subMonitor;
    pthread_t ntid_dealData;
    pthread_create(&ntid_subReactor, NULL, pthread_subReactor, (void *)"start subreactor!");
    pthread_create(&ntid_subMonitor, NULL, pthread_monitor, (void *)"start monitor!");



//    pthread_create(&ntid_dealData, NULL, pthread_dealData, (void *)"start dealDate!");

    int pool_num = 5;
    threadpool_t pool;
    //初始化线程池，最多五个线程
    threadpool_init(&pool, pool_num);
    for (int i = 0; i < pool_num; i++)
    {
        threadpool_add_task(&pool, pthread_dealData, (void *)"start dealDate!");
    }


    //printf("waiting client...\n");
    printf("start mainreactor!\n");

    while (1)
    {
        nfds = epoll_nfds();
        for (int i = 0; i < nfds; i++)
        {
            que_evs.push(events[i]);
        }
        //sleep(1);

    }

    epoll_close();


    return 0;
}
