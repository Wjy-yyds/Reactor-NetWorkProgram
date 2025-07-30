/**
 * Project 66th
 */
#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H
#include "Socket.h"
#include "InetAddress.h"
#include <string>

using std::string;
class Acceptor {
public: 

Acceptor(const string & ip, unsigned short port);
    
~Acceptor();
//.h
void ready();
    
int acceptfd();

int getListenFd();
private: 
    Socket _sock;
    InetAddress _addr;
    
void setReuseAddr();
    
void setReusePort();
    
void bindSocket();
    
void listenfd();

};

#endif //_ACCEPTOR_H