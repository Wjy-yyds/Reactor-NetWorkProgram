/**
 * Project 66th
 */


#include "Acceptor.h"
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>          /* See NOTES */
#include <iostream>

/**
 * Acceptor implementation
 */


/**
 * @param ip
 * @param port
 */
Acceptor::Acceptor(const string & ip, unsigned short port):_sock(),_addr(ip,port){

}

Acceptor::~Acceptor() {

}

//cpp
void Acceptor::ready() {
    setReuseAddr();
    setReusePort();
    bindSocket();
    listenfd();
    std::cout<<"Acceptor ready"<<std::endl;
}

//accept函数用来接受连接请求，返回一个新的文件描述符，用于与客户端通信
int Acceptor::acceptfd() {
    int connfd = ::accept(_sock.getFd(),nullptr,nullptr);
    if(connfd == -1)
    {
        perror("accept");
        return -1;
    }
    return connfd;
}

/**
 * @return void
 */
void Acceptor::setReuseAddr() {
    int opt = 1;
    int retval = setsockopt(_sock.getFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    return;
}

/**
 * @return void
 */
void Acceptor::setReusePort() {
    int opt2 = 1;
    int retval = setsockopt(_sock.getFd(), SOL_SOCKET, SO_REUSEPORT, &opt2, sizeof(opt2));
    return;
}

/**
 * @return void
 */
void Acceptor::bindSocket() {
    int ret = bind(_sock.getFd(), (struct sockaddr*)_addr.getInetAddressPtr(), sizeof(*_addr.getInetAddressPtr()));
    if(ret >=0){
        return;
    }else{
        perror("bind error!");
        return;
    }
}

/**
 * @return void
 */
void Acceptor::listenfd() {
    int ret = listen(_sock.getFd(), 128);
    if(ret >=0){
        return;
    }else{
        perror("listen error!");
        return;
    }
    
}

//服务器用于监听的fd
int Acceptor::getListenFd(){
    return _sock.getFd();
}