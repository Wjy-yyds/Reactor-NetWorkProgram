#include "TaskQueue.h"
#include <functional>
#include <mutex>
#include <thread>

template <typename T> TaskQueue<T>::TaskQueue(size_t s) : capacity(s) {}
template <typename T> TaskQueue<T>::~TaskQueue() {}
template <typename T> void TaskQueue<T>::push(T &&t) {
  unique_lock<mutex> uq(mut);
  while (full()) {
    notFull.wait(uq);
  }
  que.push(t);

  notEmpty.notify_all();
  this_thread::sleep_for(chrono::milliseconds(100));
}
template <typename T> T TaskQueue<T>::pop() {
  unique_lock<mutex> uq(mut);
  while (empty() && m_flag) {
    notEmpty.wait(uq);
  }
  if (m_flag) {
    T temp = que.front();
    que.pop();
    notFull.notify_one();
    this_thread::sleep_for(chrono::milliseconds(100));
    return temp;
  } else {
    return nullptr;
  }
}
//满返回true
template <typename T> bool TaskQueue<T>::full() {
  return capacity == que.size();
}
//空返回true
template <typename T> bool TaskQueue<T>::empty() { return !que.size(); }

template <typename T> void TaskQueue<T>::wakeup() { 
    m_flag = false;
    notEmpty.notify_all();
 }
template class TaskQueue<function<void()>>;