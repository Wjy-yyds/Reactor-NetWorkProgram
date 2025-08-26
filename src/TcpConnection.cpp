/**
 * Project 66th
 */


#include "TcpConnection.h"
#include <iostream>
#include <sstream>
using std::ostringstream;
using std::cout;
using std::endl;

/**
 * TcpConnection implementation
 */


/**
 * @param fd
 */
TcpConnection::TcpConnection(int fd)
    : _sockIO(fd)
    , _sock(fd)
    , _localAddr(getLocalAddr())
      , _peerAddr(getPeerAddr())
{

}

TcpConnection::~TcpConnection() {

}

//进行一次复制式的预读取
//如果读到的字符数为0，代表连接已经断开了
bool TcpConnection::isClosed()
{
    char buff[5] = {0};
    int ret = ::recv(_sock.getFd(),buff,sizeof(buff),MSG_PEEK);
    return 0 == ret;
}


/**
 * @return string
 */
string TcpConnection::receive() {
    char buff[65535] = {0};
    _sockIO.readLine(buff,sizeof(buff));
    return string(buff);
}

/**
 * @param msg
 * @return void
 */
void TcpConnection::send(const string & msg) {
    _sockIO.writen(msg.c_str(),msg.size());
}

//打印连接的信息
string TcpConnection::toString()
{
    ostringstream oss;
    oss << _localAddr.getIp() << ":"
        << _localAddr.getPort() << " =====> "
        << _peerAddr.getIp() << ":"
        << _peerAddr.getPort();

    return oss.str();
}

//获取本端与对端的ip地址和端口号
InetAddress TcpConnection::getLocalAddr()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(sockaddr);
    int ret = getsockname(_sock.getFd(),(struct sockaddr *)&addr,&len);
    if(ret == -1)
    {
        perror("getsockname");
    }

    return InetAddress(addr);
}

InetAddress TcpConnection::getPeerAddr()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(sockaddr);
    int ret = getpeername(_sock.getFd(),(struct sockaddr *)&addr,&len);
    if(ret == -1)
    {
        perror("getpeername");
    }

    return InetAddress(addr);

}


void TcpConnection::setNewConnectionCallback(const TcpConnectionCallback & cb)
{
    //此处不能使用移动语义
    //否则建立第一个连接，就让EventLoop的function数据成员变成了空的
    //后续再建立连接，进行转接，也只能转发空的function
    /* _onNewConnection = std::move(cb); */
    _onNewConnection = cb;
}
void TcpConnection::setMessageCallback(const TcpConnectionCallback & cb)
{
    _onMessage = cb;
}
void TcpConnection::setCloseCallback(const TcpConnectionCallback & cb)
{
    _onClose = cb;
}

void TcpConnection::handleNewConnectionCallback()
{
    if(_onNewConnection)
    {
        //需要传入的参数是一个管理本对象的shared_ptr智能指针
        //要使用的合法方式，考虑用到辅助类
        _onNewConnection(shared_from_this());
    }
    else
    {
        cout << "_onNewConnection == nullptr" << endl;
    }
}
void TcpConnection::handleMessageCallback()
{
    if(_onMessage)
    {
        _onMessage(shared_from_this());
    }
    else
    {
        cout << "_onMessage == nullptr" << endl;
    }
}

void TcpConnection::handleCloseCallback()
{
    if(_onClose)
    {
        _onClose(shared_from_this());
    }
    else
    {
        cout << "_onClose == nullptr" << endl;
    }
}
