/**
 * Project 66th
 */


#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include "Acceptor.h"
#include "TcpConnection.h"
#include <sys/epoll.h>
#include <vector>
#include <map>
#include <memory>
#include <functional>
using std::vector;
using std::map;
using std::shared_ptr;
using std::function;
using std::bind;

using TcpConnectionPtr = shared_ptr<TcpConnection>;
using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>;


class EventLoop {
public: 
    /**
     * @param acc
     */
    EventLoop(Acceptor & acc);

    ~EventLoop();

    void loop();

    void unloop();

    void setNewConnectionCallback(TcpConnectionCallback && cb);
    void setMessageCallback(TcpConnectionCallback && cb);
    void setCloseCallback(TcpConnectionCallback && cb);
private: 
    int createEpollFd();

    void addEpollReadFd(int fd);

    void delEpollReadFd(int fd);

    void waitEpollFd();

    void handleNewConnection();

    /**
     * @param fd
     */
    void handleMessage(int fd);


private:
    int _epfd;
    vector<struct epoll_event> _evlist;
    bool _isLooping;
    Acceptor & _acceptor;
    map<int,shared_ptr<TcpConnection>> _conns;

    TcpConnectionCallback _onNewConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;
};

#endif //_EVENTLOOP_H
