#include "EventLoop.h"
#include "TcpConnection.h"
#include <asm-generic/errno-base.h>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <sys/epoll.h>
#include <unistd.h>
#include <utility>

//需要先创建Acceptor,并获取用于监听是否有连接的fd
//把listenfd添加到监听集合中去
EventLoop::EventLoop(Acceptor &acceptor)
    : _epfd(createEpollFd()), _evtList(), _isLooping(true), _acceptor(acceptor),
      _conns() {
  _evtList.resize(10);
  int listenFd = _acceptor.getListenFd();
  addEpollReadFd(listenFd);
}

EventLoop::~EventLoop() { close(_epfd); }
//循环wait
void EventLoop::loop() {
  _isLooping = true;
  while (_isLooping) {
    waitEpollFd();
  }
}
void EventLoop::unloop() { _isLooping = false; }

// accept返回的fd是新连接到来的fd
//注意：把vector当作数组指针使用
void EventLoop::waitEpollFd() {

  int nready = 0;
  do {
    nready = epoll_wait(_epfd, _evtList.data(), _evtList.size(), 10000);
  } while (-1 == nready && errno == EINTR);

  if (-1 == nready) {
    std::cerr << "epoll_wait error" << strerror(errno);
    return;
  } else if (0 == nready) {
    std::cout << "epoll监听集合无就绪事件" << std::endl;
  } else {

    if (nready == (int)_evtList.size()) {
      _evtList.resize(2 * nready);
    }
    for (int i = 0; i < nready; ++i) {
      int sockfd = _acceptor.getListenFd();
      int activefd = _evtList[i].data.fd;
      if (sockfd == activefd) {
        handleNewConnection();
      } else {
        handleNewMessage(activefd);
      }
    }
  }
}
//处理新连接 1.获取连接符connfd
// 2.利用connfd创建TcpConnection对象,添加到map
// 3.添加到epoll监听中
void EventLoop::handleNewConnection() {
  int connfd = _acceptor.acceptfd();
  if (-1 == connfd) {
    perror("handle acceptor");
  }
  _conns.insert(
      std::make_pair(connfd, std::make_shared<TcpConnection>(connfd)));
  std::cout << _conns[connfd]->toString() << std::endl;
  _conns[connfd]->setOnNewConnectionCallback(_onNewConnection);
  _conns[connfd]->setOnCloseCallback(_onClose);
  _conns[connfd]->setOnMessageCallback(_onMessage);
  addEpollReadFd(connfd);
  _conns[connfd]->handleNewConnectionCallback();
}

void EventLoop::handleNewMessage(int fd) {
  auto it = _conns.find(fd);
  std::cout << it->second->toString() << std::endl;
  //_onMessage = std::bind(&TcpConnection::handleNewMessageCallback,it->second);
  if (it != _conns.end()) {
    // bool flag = it->second->isClosed();
    // std::string msg = it->second->receive();
    // std::cout<<"Server receive"<<msg<<std::endl;
    // msg.insert(0,"Server return:");
    // it->second->send(msg);
    //it->second->handleNewMessageCallback();
    bool flag = it->second->isClosed();
    if(flag){
      it->second->handleCloseCallback();
      delEpollReadFd(fd);
      _conns.erase(fd);
    }else{
      it->second->handleNewMessageCallback();
    }
  }
}
//使用epoll_create创建eopllfd监听集合,并初始化_epfd
int EventLoop::createEpollFd() { return ::epoll_create(1); }
//往_evList中添加监听
void EventLoop::addEpollReadFd(int fd) {
  struct epoll_event event;
  memset(&event, 0, sizeof(event));
  event.data.fd = fd;
  event.events = EPOLLIN;
  ::epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &event);
}
//删除fd的struct
void EventLoop::delEpollReadFd(int fd) {

  ::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
}

void EventLoop::setOnMessageCallback(TcpConnectionCallback &&cb) {
  _onMessage = std::move(cb);
}

void EventLoop::setOnNewConnectionCallback(TcpConnectionCallback &&cb) {
  _onNewConnection = std::move(cb);
}
void EventLoop::setOnCloseCallback(TcpConnectionCallback &&cb) {
  _onClose = std::move(cb);
}
