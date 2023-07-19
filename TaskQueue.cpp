#include"TaskQueue.h"

TaskQueue::TaskQueue()
{
    pthread_mutex_init(&m_mutex,NULL);
}
TaskQueue::~TaskQueue(){
    pthread_mutex_destroy(&m_mutex);
}
void TaskQueue::addTask(Task task){
    pthread_mutex_lock(&m_mutex);
    m_taskQ.push(task);
    pthread_mutex_unlock(&m_mutex);
}
Task TaskQueue::takeTask(){
    Task t;
    if(m_taskQ.size()==0) return t;
    pthread_mutex_lock(&m_mutex);
    t = m_taskQ.front();
    m_taskQ.pop();
    pthread_mutex_unlock(&m_mutex);
    return t;
}
void TaskQueue::addTask(callback f,void *arg){
    pthread_mutex_lock(&m_mutex);
    m_taskQ.push(Task(f,arg));
    pthread_mutex_unlock(&m_mutex);
}