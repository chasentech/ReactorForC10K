#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>


//线程池状态
typedef struct condition
{
    pthread_mutex_t pmutex;	//互斥锁
    pthread_cond_t pcond;	//条件变量
}condition_t;


//封装线程池中的对象需要执行的任务对象
typedef struct task
{
    void *(*run)(void *args);  //函数指针，需要执行的任务
    void *arg;              //参数
    struct task *next;      //任务队列中下一个任务
}task_t;


//线程池结构体
typedef struct threadpool
{
    condition_t ready;   //状态量
    task_t *first;       //任务队列中第一个任务
    task_t *last;        //任务队列中最后一个任务
    int counter;         //线程池中已有线程数
    int idle;            //线程池中空闲线程数
    int max_threads;     //线程池最大线程数
    int quit;            //是否退出标志
}threadpool_t;


//线程池初始化
void threadpool_init(threadpool_t *pool, int threads);
//增加一个任务到线程池
void threadpool_add_task(threadpool_t *pool, void *(*run)(void *arg), void *arg);
//线程池销毁
void threadpool_destroy(threadpool_t *pool);





#endif // THREADPOOL_H
