/**
 * Project 66th
 */


#include "SocketIO.h"

/**
 * SocketIO implementation
 */


/**
 * @param fd
 */
SocketIO::SocketIO(int fd)
: _fd(fd)
{

}

SocketIO::~SocketIO() {
    /* close(_fd); */
}


//设置的任务可能是读取很多个字符
//也许会超过一次read能够读取的上限
//
//比如要读取10000个字符，但是单次read只能读1000
int SocketIO::readn(char *buf, int len) //10000
{
    int left = len; //10000
    char *pstr = buf; //读缓存区的首地址
    int ret = 0;

    //因为read函数单次读取的数据量存在上限
    while(left > 0)
    {
        ret = ::read(_fd, pstr, left);
        //对当前进程（线程）而言发生了中断
        //中断触发，导致CPU的使用权被抢夺，后续应该继续执行
        if(-1 == ret && errno == EINTR)
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
            break;
        }
        else
        {
            pstr += ret; //读缓冲区放了第一次读到的1000个字符，然后指针偏移
            left -= ret; //10000 - 1000 = 9000  left是剩余要读取的字符数
        }
    }

    return len - left;
}

int SocketIO::readLine(char *buf, int len)
{
    int left = len - 1;
    char *pstr = buf;
    int ret = 0, total = 0;

    while(left > 0)
    {
        //MSG_PEEK不会将缓冲区中的数据进行清空,只会进行拷贝操作
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
        }
        else
        {
            for(int idx = 0; idx < ret; ++idx)
            {
                if(pstr[idx] == '\n')
                {
                    int sz = idx + 1;
                    readn(pstr, sz);
                    pstr += sz;
                    *pstr = '\0';//C风格字符串以'\0'结尾

                    return total + sz;
                }
            }

            readn(pstr, ret);
            total += ret;
            pstr += ret;
            left -= ret;
        }
    }
    *pstr = '\0';

    return total;

}

int SocketIO::writen(const char *buf, int len)
{
    int left = len;
    const char *pstr = buf;
    int ret = 0;

    //同样，一次write能写的数据量也有上限
    while(left > 0)
    {
        ret = write(_fd, pstr, left);
        if(-1 == ret && errno == EINTR)
        {
            continue;
        }
        else if(-1 == ret)
        {
            perror("writen error -1");
            return -1;
        }
        else if(0 == ret)
        {
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

