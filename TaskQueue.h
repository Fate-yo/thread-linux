#include<queue>
#include <thread>
using callback = void (*)(void*);
struct Task {

    Task (){
        function = nullptr;
        arg = nullptr;
    }
    Task(callback f,void *arg){
        this->arg = arg;
        function = f;
    }
    callback function;
    void *arg;

};
class TaskQueue{
public:
    TaskQueue();
    ~TaskQueue();
    void addTask(Task task);
    Task takeTask();
    void addTask(callback f,void *arg);
    inline size_t gettaskNumber(){
        return m_taskQ.size();
    }
private:
    pthread_mutex_t m_mutex;
    std::queue<Task>m_taskQ;
};