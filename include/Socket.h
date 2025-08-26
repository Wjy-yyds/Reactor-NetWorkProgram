/**
 * Project 66th
 */


#ifndef _SOCKET_H
#define _SOCKET_H

#include "NonCopyable.h"
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>


class Socket: public NonCopyable {
public: 

    Socket();

    /**
     * @param fd
     */
    explicit Socket(int fd);

    ~Socket();

    int getFd();
private: 
    int _fd;
};

#endif //_SOCKET_H
