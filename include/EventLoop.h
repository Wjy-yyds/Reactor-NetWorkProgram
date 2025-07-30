#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H
#include "Acceptor.h"
#include "TcpConnection.h"
#include <memory>
#include <vector>
#include <map>
#include <sys/epoll.h>
#include <functional>
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using TcpConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
class EventLoop{
    private:
    int _epfd;//epoll_create函数的返回结果，创建的监听集合
    std::vector<struct epoll_event> _evtList;//epoll_wait函数的第二个参数，数需要将就绪的文件描述符放在struct epoll_event结构体
    bool _isLooping; //用于判断事件循环是否继续
    Acceptor & _acceptor;//Acceptor的引用，可以获取连接的fd
    std::map<int, std::shared_ptr<TcpConnection>> _conns;//每个fd对应一个连接,share_ptr方便资源管理
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onNewConnection;
    TcpConnectionCallback _onClose;


    public:
    EventLoop(Acceptor & acceptor);
    ~EventLoop();
    void loop();
    void unloop();
    void setOnMessageCallback(TcpConnectionCallback && cb);
    void setOnNewConnectionCallback(TcpConnectionCallback && cb);
    void setOnCloseCallback(TcpConnectionCallback && cb);

    private:
    void waitEpollFd();
    void handleNewConnection();
    void handleNewMessage(int fd);
    int createEpollFd();
    void addEpollReadFd(int);
    void delEpollReadFd(int); 
    
};
// EventLoop(Acceptor & acceptor):_epfd(0),
    // _evtList(),
    // _isLooping(true),
    // _acceptor(acceptor),
    // _conns(){
    //     _evtList.reserve(10);
    // }
#endif