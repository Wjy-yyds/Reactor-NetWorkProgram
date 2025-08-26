/**
 * Project 66th
 */


#ifndef _TCPCONNECTION_H
#define _TCPCONNECTION_H

#include "SocketIO.h"
#include "Socket.h"
#include "InetAddress.h"
#include <string>
#include <memory>
#include <functional>
using std::string;
using std::shared_ptr;
using std::function;

class TcpConnection;

using TcpConnectionPtr = shared_ptr<TcpConnection>;
using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>;

class TcpConnection 
: public std::enable_shared_from_this<TcpConnection>
{
public: 

    /**
     * @param fd
     */
    explicit TcpConnection(int fd);

    ~TcpConnection();

    bool isClosed();

    string receive();

    /**
     * @param msg
     */
    void send(const string & msg);
    
    //打印连接的信息
    string toString();

    //这三个set函数中不能使用移动语义，转接EventLoop的function
    //所以形参建议不使用右值引用，而是使用const左值引用
    void setNewConnectionCallback(const TcpConnectionCallback & cb);
    void setMessageCallback(const TcpConnectionCallback & cb);
    void setCloseCallback(const TcpConnectionCallback & cb);

    void handleNewConnectionCallback();
    void handleMessageCallback();
    void handleCloseCallback();

private:
    //获取本端与对端的ip地址和端口号
    InetAddress getLocalAddr();
    InetAddress getPeerAddr();

private: 
    SocketIO _sockIO;

    //为了打印信息加入的数据成员
    Socket _sock;
    InetAddress _localAddr;
    InetAddress _peerAddr;

    //回调
    TcpConnectionCallback _onNewConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;
};

#endif //_TCPCONNECTION_H
