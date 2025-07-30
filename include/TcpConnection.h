/**
 * Project 66th
 */


#ifndef _TCPCONNECTION_H
#define _TCPCONNECTION_H
#include <memory>
#include <string>
#include <functional>
#include "SocketIO.h"
#include "InetAddress.h"
#include "Socket.h"
using std::string;

class TcpConnection:public std::enable_shared_from_this<TcpConnection> {
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    using TcpConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
public: 
//aa   
explicit TcpConnection(int fd);
    
~TcpConnection();
    
string receive();

    string toString();
    void send(const string & msg);
    void handleNewMessageCallback();
    void setOnMessageCallback(const TcpConnectionCallback & cb);
    bool isClosed();
    void setOnNewConnectionCallback(const TcpConnectionCallback & cb);
    void setOnCloseCallback(const TcpConnectionCallback & cb);
    void handleCloseCallback();
    void handleNewConnectionCallback();

private:
    //获取本端与对端的ip地址和端口号
    InetAddress getLocalAddr();
    InetAddress getPeerAddr();
    
    


private: 
    SocketIO _sockIO;

    Socket  _sock;
    InetAddress _localAddr;
    InetAddress _peerAddr;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;
    TcpConnectionCallback _onNewConnection;
};

#endif //_TCPCONNECTION_H