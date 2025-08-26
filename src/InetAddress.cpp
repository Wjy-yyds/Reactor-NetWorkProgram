/**
 * Project 66th
 */


#include "InetAddress.h"

/**
 * InetAddress implementation
 */


/**
 * @param ip
 * @param port
 */
InetAddress::InetAddress(const string & ip, unsigned short port) {
    memset(&_addr, 0, sizeof(_addr));//初始化seraddr
    _addr.sin_family = AF_INET;
    
    //本机字节序转换为网络字节序(包括ip与端口号)
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = inet_addr(ip.c_str());//把本机的ip转到网络
}

/**
 * @param addr
 */
InetAddress::InetAddress(const struct sockaddr_in & addr)
: _addr(addr)
{

}

InetAddress::~InetAddress() {

}

/**
 * @return string
 */
string InetAddress::getIp() {
    return string(inet_ntoa(_addr.sin_addr));
}

/**
 * @return unsigned short
 */
unsigned short InetAddress::getPort() {
    return ntohs(_addr.sin_port);
}

/**
 * @return struct sockaddr_in *
 */
const struct sockaddr_in * InetAddress::getInetAddressPtr() {
    return &_addr;
}
