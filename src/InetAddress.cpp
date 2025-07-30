/**
 * Project 66th
 */


#include "InetAddress.h"
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

/**
 * InetAddress implementation
 */

//const string可以接受const char*和右值
InetAddress::InetAddress(const string & ip, unsigned short port) {
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = inet_addr(ip.c_str());
}


InetAddress::InetAddress(const struct sockaddr_in & addr){
    //c++还需要重载运算符=
    memcpy(&_addr, &addr, sizeof(_addr));
}

InetAddress::~InetAddress() {

}


string InetAddress::getIp() {
    char ipStr[32];
    inet_ntop(AF_INET, &_addr.sin_addr, ipStr, 32);
    return std::string(ipStr);
    // return std::to_string(_addr.sin_addr.s_addr);
}


unsigned short InetAddress::getPort() {
    auto pport = ntohs(_addr.sin_port);

    return pport;
}


struct sockaddr_in * InetAddress::getInetAddressPtr() {
    return &_addr;
}