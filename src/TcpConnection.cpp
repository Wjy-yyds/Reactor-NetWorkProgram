/**
 * Project 66th
 */

#include "TcpConnection.h"
#include "EventLoop.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
/**
 * TcpConnection implementation
 */

/**
 * @param fd accept函数的返回值
 */
TcpConnection::TcpConnection(int fd)
    : _sockIO(fd), _sock(fd), _localAddr(getLocalAddr()),
      _peerAddr(getPeerAddr()) {
  // setOnMessageCallback(std::bind(&TcpConnection::handleNewMessageCallback,this));
}

TcpConnection::~TcpConnection() {}

/**
 * @return string
 */
string TcpConnection::receive() {
  char buf[65535];
  memset(buf, 0, 65535);
  int ret = _sockIO.readLine(buf, sizeof(buf));
  // setOnMessageCallback(std::bind(&TcpConnection::handleNewMessageCallback,this));
  if (ret <= 0) {
    return std::string();
  }
  return std::string(buf);
}

/**
 * @param msg
 * @return void
 */
void TcpConnection::send(const string &msg) {
  _sockIO.writen(msg.c_str(), msg.size());
  return;
}

//获取本端与对端的ip地址和端口号
InetAddress TcpConnection::getLocalAddr() {
  struct sockaddr_in addr;
  socklen_t len = sizeof(sockaddr);
  int ret = getsockname(_sock.getFd(), (struct sockaddr *)&addr, &len);
  if (ret == -1) {
    perror("getsockname");
  }

  return InetAddress(addr);
}

InetAddress TcpConnection::getPeerAddr() {
  struct sockaddr_in addr;
  socklen_t len = sizeof(sockaddr);
  int ret = getpeername(_sock.getFd(), (struct sockaddr *)&addr, &len);
  if (ret == -1) {
    perror("getpeername");
  }

  return InetAddress(addr);
}

//打印连接的信息
string TcpConnection::toString() {
  std::ostringstream oss;
  oss << _localAddr.getIp() << ":" << _localAddr.getPort() << " =====> "
      << _peerAddr.getIp() << ":" << _peerAddr.getPort();

  return oss.str();
}

void TcpConnection::handleNewMessageCallback() {
  if (_onMessage) {
    _onMessage(shared_from_this());
  } else {
    std::cout << "_onMessage is null" << std::endl;
  }
}

//不能写成右值引用，会导致eventloop无法获取到_onMessage的地址
//接下来其他类也需要调用这个函数，所以要写成左值引用
void TcpConnection::setOnMessageCallback(const TcpConnectionCallback &cb) {
  _onMessage = cb;
}

bool TcpConnection::isClosed() {
  char buff[5] = {0};
  int ret = ::recv(_sock.getFd(), buff, sizeof(buff), MSG_PEEK);
  return 0 == ret;
}
void TcpConnection::setOnNewConnectionCallback(
    const TcpConnectionCallback &cb) {
  _onNewConnection = cb;
}
void TcpConnection::setOnCloseCallback(const TcpConnectionCallback &cb) {
  _onClose = cb;
}
void TcpConnection::handleCloseCallback() {
  if (_onClose) {
    _onClose(shared_from_this());
  }
}
void TcpConnection::handleNewConnectionCallback() {
  if (_onNewConnection) {
    _onNewConnection(shared_from_this());
  }
}
