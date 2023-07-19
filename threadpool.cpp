#include"threadpool.h"

using namespace std;
const int Number = 2;


ThreadPool::ThreadPool(int min, int max){
    do{   
        TaskQ = new TaskQueue; 
        if (TaskQ == nullptr)
        {
            cout<<"malloc TaskQ fail..."<<endl;
            break;
        }
        ThreadIDs = new pthread_t[max];
        if (ThreadIDs == nullptr)
        {
            cout<<"malloc ThreadIDs fail..."<<endl;
            break;
        }
        memset(ThreadIDs, 0, sizeof(pthread_t) * max);
        minNum = min;
        maxNum = max;
        busyNum = 0;
        liveNum = min;    // 和最小个数相等
        exitNum = 0;

        if (pthread_mutex_init(&mutexpool, NULL) != 0 ||pthread_cond_init(&notEmpty, NULL) != 0 )
        {
            cout<<"mutex or condition init fail..."<<endl;
            break;
        }

        // 任务队列
        shutdown = false;
     
        // 创建线程
        pthread_create(&managerID, NULL, manager, this);
        for (int i = 0; i < min; i++) {
             cout << "创建子线程, ID: " << to_string(ThreadIDs[i]) << endl;
            pthread_create(&ThreadIDs[i], NULL, worker, this);
        }
        return;
    } while (0);
    // 释放资源
    if (ThreadIDs) delete []ThreadIDs;
    if (TaskQ) delete TaskQ;

}
ThreadPool::~ThreadPool() {
 
    // 关闭线程池
    shutdown = true;
    //唤醒管理线程
    pthread_join(managerID, NULL);
    //唤醒阻塞消费线程
    for (int i = 0; i < liveNum; i++) {
        pthread_cond_signal(&notEmpty);
    }
    if (TaskQ) {
        delete TaskQ;
    }
    if (ThreadIDs) {
        delete []ThreadIDs;
    }
    pthread_mutex_destroy(&mutexpool);
 
    pthread_cond_destroy(&notEmpty);
}
void ThreadPool::addTask(Task task) {
    if (shutdown) {   
        return;
    }
    TaskQ->addTask(task);
    pthread_cond_signal(&notEmpty);
}

int ThreadPool::GetBusyNum() {
    pthread_mutex_lock(&mutexpool);
    int busyNum = this->busyNum;
    pthread_mutex_lock(&mutexpool);
    return busyNum;
}

int ThreadPool::getAliveNum() {
    pthread_mutex_lock(&mutexpool);
    int liveNum = this->liveNum;
    pthread_mutex_lock(&mutexpool);
    return liveNum;
}

void *ThreadPool::worker(void *arg) {
    ThreadPool *pool = static_cast <ThreadPool *>(arg);
    while (true) {
        
        pthread_mutex_lock(&pool->mutexpool);
        while (pool->TaskQ->gettaskNumber() == 0 && !pool->shutdown) {
              
            pthread_cond_wait(&pool->notEmpty, &pool->mutexpool);
            if (pool->exitNum > 0) {
                pool->exitNum--;
                if (pool->liveNum > pool->minNum) {
                    pool->liveNum--;
                    pthread_mutex_unlock(&pool->mutexpool);
                    pool->ThreadExit();
                }
            }
        }
        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->mutexpool);
            pool->ThreadExit();
        }
        Task task = pool->TaskQ->takeTask();
        pool->busyNum++;
        cout<<"thread: "<<to_string(pthread_self())<<" start working .... "<<endl; 
        pthread_mutex_unlock(&pool->mutexpool);
        task.function(task.arg);
        delete task.arg;
        task.arg = nullptr;
        
        pthread_mutex_lock(&pool->mutexpool);
        pool->busyNum--;
        cout<<"thread: "<<to_string(pthread_self())<<" end working .... "<<endl; 
        pthread_mutex_unlock(&pool->mutexpool);
    }
    return NULL;
}
 
void* ThreadPool::manager (void *arg) {
    ThreadPool *pool = static_cast<ThreadPool*> (arg);
    while (!pool->shutdown) {
         
        sleep(5);
        pthread_mutex_lock(&pool->mutexpool);
        int queueSize = pool->TaskQ->gettaskNumber();
        int liveNum = pool->liveNum;
        int busyNum = pool->busyNum;
        pthread_mutex_unlock(&pool->mutexpool);
        // 添加线程添加2
        if (queueSize > liveNum && liveNum < pool->maxNum) {
            int counter = 0;
            pthread_mutex_lock(&pool->mutexpool);
            for (int i = 0; i < pool->maxNum && counter < Number && liveNum < pool->maxNum; i++) {
                if (pool->ThreadIDs[i] == 0) {
                    pthread_create(&pool->ThreadIDs[i], NULL, worker, pool);
                    cout << "manager add thread, ID: " << to_string(pool->ThreadIDs[i]) << endl;
                    counter++;
                    liveNum++;
                }
            }
            pthread_mutex_unlock(&pool->mutexpool);
        }
        // 销毁线程 忙的线程*2 < 存活的 && 存活 > 最小
        if (busyNum * 2 < liveNum && liveNum > pool->minNum) {
            pthread_mutex_lock(&pool->mutexpool);
            pool->exitNum = Number;
            pthread_mutex_unlock(&pool->mutexpool);
            for (int i = 0; i < Number; i++) {
                cout << "manager delete thread, ID: " << to_string(pool->ThreadIDs[i]) << endl;
                   
                pthread_cond_signal(&pool->notEmpty);
            }
        }
    }
    return nullptr;
}

void ThreadPool::ThreadExit() {
    pthread_t tid = pthread_self();
    for (int i = 0; i < maxNum; i++) {
        if (ThreadIDs[i] == tid) {
            ThreadIDs[i] = 0;
            cout<<"thread "<<to_string(pthread_self())<<" exit ...."<<endl;
            break;
        }
    }
    pthread_exit(NULL);
}
