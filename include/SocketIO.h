/**
 * Project 66th
 */


#ifndef _SOCKETIO_H
#define _SOCKETIO_H
#include <sys/types.h>
#include <sys/socket.h>
class SocketIO {
public: 
    
explicit SocketIO(int fd);
    
~SocketIO();
    

int readn(char * buf, int len);
    

int readLine(char * buf, int len);
    

int writen(const char * buf, int len);
private: 
    int _fd;
};

#endif //_SOCKETIO_H