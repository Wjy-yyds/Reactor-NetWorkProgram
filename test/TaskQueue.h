#ifndef _TASKQUEUE_H
#define _TASKQUEUE_H
#include <functional>
#include <iostream>
#include <iterator>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;
template<class T>
class TaskQueue{
    private:
    size_t capacity;
    queue<T> que;
    mutex mut;
    condition_variable notEmpty;
    condition_variable notFull; 
    bool m_flag = true; 

    public:
    TaskQueue(size_t s);
    ~TaskQueue();
    void push( T && t);
    T pop();
    //满返回true
    bool full();
    //空返回true
    bool empty();
    void wakeup();
};

#endif