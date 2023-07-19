#ifndef _THREADPOOL_H
#define _THREADPOOL_H
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "TaskQueue.h"
#include <string.h>
#include <string> 
#include <unistd.h>

class ThreadPool {
public:

    ThreadPool(int min, int max);

    // 销毁线程池
    ~ThreadPool();

    // 给线程池添加任务
    void addTask(Task task);

    // 获取线程池中工作的线程的个数
    int GetBusyNum();

    // 获取线程池中活着的线程的个数
    int getAliveNum();

    //////////////////////
    // 工作的线程(消费者线程)任务函数


private:
    static void* worker(void* arg);
    // 管理者线程任务函数
    static void* manager(void* arg);
    // 单个线程退出
    void ThreadExit();
private:
    TaskQueue *TaskQ;
    pthread_t managerID;
    pthread_t *ThreadIDs;
    int minNum;
    int maxNum;
    int busyNum;
    int liveNum;
    int exitNum;
    pthread_mutex_t mutexpool;
    pthread_cond_t notEmpty;
    bool shutdown=false;
 
};


 
// 创建线程池并初始化

#endif  // _THREADPOOL_H

 
