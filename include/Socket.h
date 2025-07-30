/**
 * Project 66th
 */


#ifndef _SOCKET_H
#define _SOCKET_H
#include <string>
#include "NonCopyable.h"
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>

using std::string;

class Socket: public NonCopyable {
public: 
    
Socket();
    

explicit Socket(int fd);
    
~Socket();
    
int getFd();
private: 
    int _fd;//socket函数创造的fd
};

#endif //_SOCKET_H