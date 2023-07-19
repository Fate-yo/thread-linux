#include"threadpool.h"
 
#include <iostream>
using namespace std;

void taskfunc(void *arg){
    int num = *(int*)arg;
    printf("%d,%ld\n",num,pthread_self());
    sleep(1);
}
int main(){
    ThreadPool pool(3,10);
    for( int i=0;i<1011110;i++){
        int *num = new int(i+100);
        pool.addTask(Task(taskfunc,num));
    }
    sleep(20);
    cout<<"OK"<<endl;
 
}
