/**
 * Project 66th
 */


#include "SocketIO.h"
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
/**
 * SocketIO implementation
 */


/**
 * @param fd
 */
SocketIO::SocketIO(int fd):_fd(fd) {

}

SocketIO::~SocketIO() {

}

/*往buf中读n个字节
*/
int SocketIO::readn(char * buf, int len) {
    // int readsize = recv(_fd, buf, len, 0);
    // return readsize;

    int total =0,left =len;
    char *pstr = buf;
    int ret =0;
    while(left>0){
        ret = ::read(_fd, pstr, len);
        if(-1 == ret&& errno == EINTR)
        {
            continue;
        }
        else if(-1 == ret)
        {
            perror("read error -1");
            return -1;
        }
        else if(0 == ret)
        {
            std::cout<<"client closed"<<std::endl;
            break;
        }
        else
        {
            pstr += ret; 
            left -= ret; 
        }
    }
    return len - left;
}

/**
 * @param buf 读内容存放位置
 * @param len 读取最多长度
 * @return int 实际读取的长度
    读取一行内容
 */
int SocketIO::readLine(char * buf, int len) {
    // int total = 0;
    // char ch = '\0';

    // while(total<len-1){
    //     ssize_t n = recv(_fd, &ch, 1, 0);
    //     if(n<=0){
    //         perror("readline error");
    //         return -1;
    //     }
        
    //     buf[total++] = ch;

    //     if(ch == '\n'){
    //         break;
    //     }
    // }
    // buf[total] = '\0';
    // return total;
    int left = len -1;
    char * pstr = buf;
    int ret = 0,total =0;

    while(left>0){
        ret = recv(_fd, pstr, left, MSG_PEEK);
        if(-1 == ret && errno == EINTR)
        {
            continue;
        }
        else if(-1 == ret)
        {
            perror("readLine error -1");
            return -1;
        }
        else if(0 == ret)
        {
            break;
        }else{
            for(int idx=0;idx<ret;++idx){
                if(pstr[idx]=='\n'){
                    int sz = idx +1;
                    readn(pstr,sz);
                    pstr+=sz;
                    *pstr = '\0';
                    //后来无所谓了，直接返回就行
                    return total +sz;
                }
            }
        }
        readn(pstr, ret);
        pstr+=ret;
        total+=ret;
        left-=ret;
    }
    *pstr = '\0';
    return total;
}

/**
 * @param buf
 * @param len
 * @return int
 
 */
int SocketIO::writen(const char * buf, int len) {
    return send(_fd, buf, len, 0);
}