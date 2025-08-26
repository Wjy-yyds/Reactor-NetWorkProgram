/**
 * Project 66th
 */


#include "Acceptor.h"
#include <iostream>
using std::cout;
using std::endl;

/**
 * Acceptor implementation
 */


/**
 * @param ip
 * @param port
 */
Acceptor::Acceptor(const string & ip, unsigned short port) 
: _sock()
, _addr(ip,port)
{

}

Acceptor::~Acceptor() {

}

/**
 * @return void
 */
void Acceptor::ready() {
    setReuseAddr();
    setReusePort();
    bind();
    listen();
}

/**``
 * @return void
 */
void Acceptor::setReuseAddr() {
    int opt = 1;
    int retval = setsockopt(_sock.getFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(retval == -1)
    {
        perror("setsockopt addr");
        return;
    }
}

/**
 * @return void
 */
void Acceptor::setReusePort() {
    int opt = 1;
    int retval = setsockopt(_sock.getFd(), SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    if(retval == -1)
    {
        perror("setsockopt port");
        return;
    }
}

/**
 * @return void
 */
void Acceptor::bind() {
    //告诉编译器，这里调用的bind是C语言的库函数
    int ret = ::bind(_sock.getFd(),
                   (struct sockaddr*)_addr.getInetAddressPtr(),
                    sizeof(struct sockaddr));       
    if(ret == -1)
    {
        perror("bind");
        return;
    }
}

/**
 * @return void
 */
void Acceptor::listen() {
    int ret = ::listen(_sock.getFd(),128);
    if(ret == -1)
    {
        perror("listen");
        return;
    }

    cout << "listen ..." << endl;
}

/**
 * @return int
 */
int Acceptor::accept() {
    int connfd = ::accept(_sock.getFd(),nullptr,nullptr);
    if(connfd == -1)
    {
        perror("accept");
        return -1;
    }
    return connfd;
}

int Acceptor::fd()
{
   return _sock.getFd();
}
