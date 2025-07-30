#ifndef _THREAD_POOL
#define _THREAD_POOL

#include "TaskQueue.h"
#include <cstddef>
#include <functional>
#include <vector>
#include <thread>
#include <unistd.h>
using namespace std;
using Task = function<void()>;
class ThreadPool{
    private:
    size_t threadNum;
    vector<thread> threads;
    size_t queSize;
    TaskQueue<Task> taskQueue;
    bool isExit;

    public:
    ThreadPool(size_t threadnum,size_t size):threadNum(threadnum),queSize(size)
    ,threads(),taskQueue(size),isExit(false)
    {

    }
    ~ThreadPool(){

    }
    void start() {
      for (size_t idx = 0; idx < threadNum; ++idx) {
        //thread不能拷贝构造
        threads.push_back(thread(&ThreadPool::doTask,this));
      }
    }

    void stop(){
        while(!taskQueue.empty()){
            //sleep(1);
            doTask();
        }
        
        isExit = true;

        
        taskQueue.wakeup();

        //让主线程等待子线程退出
        for (auto &th : threads) {
          th.join();
        }
    }
    void addTask(Task && task){
        if(task){
            taskQueue.push(move(task));
        }
    }
    

    Task getTask(){
        return taskQueue.pop();
    }
    //线程池中每个线程做的事情
    void doTask(){
        while (!isExit) {
            Task ptask = getTask();
            if(ptask){
                ptask();
            }else{
                cout<<"taskqueue is empty!"<<endl;
            }
        }
    }

};
#endif