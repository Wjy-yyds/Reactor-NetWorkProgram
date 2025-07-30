#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <iostream>
#include <iterator>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void errorCheck(int x) {}

int ServerHost(std::string ip, std::string port) {
  // 1. 服务端先建立一个监听的文件描述符
  // 使用 socket 函数创建一个基于 IPv4 协议、面向连接的 TCP 套接字
  // AF_INET 表示使用 IPv4 地址族
  // SOCK_STREAM 表示使用面向连接的 TCP 协议
  // 0 表示让系统自动选择合适的协议
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  errorCheck(sockfd);
  int sockopt = 1;

  //解决timewait问题
  // sockopt为1表示开启，0表示关闭
  int retval =
      setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt));
  errorCheck(retval);

  retval =
      setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &sockopt, sizeof(sockopt));
  errorCheck(retval);

  // 绑定服务端的地址信息结构体
  // 定义一个 sockaddr_in 类型的结构体，用于存储服务端的网络地址信息
  struct sockaddr_in serveraddress;
  memset(&serveraddress, 0, sizeof(serveraddress));
  // 设置地址族为 IPv4
  serveraddress.sin_family = AF_INET;
  // 将命令行参数中传入的端口号字符串转换为整数，再从主机字节序转换为网络字节序
  // atoi(argv[2]) 将字符串转换为整数
  // htons 函数将主机字节序转换为网络字节序
  serveraddress.sin_port = htons(atoi(port.c_str()));
  // 将命令行参数中传入的点分十进制 IP 地址字符串转换为网络字节序的 32 位整数
  // inet_addr 函数完成字符串到网络字节序整数的转换
  serveraddress.sin_addr.s_addr = inet_addr(ip.c_str());

  //2.绑定服务端的地址信息结构体
  //bind时传入通用数据结构体sockaddr
  int bindret = bind(sockfd, (struct sockaddr *)&serveraddress, sizeof(serveraddress));
  errorCheck(bindret);

  //3.将文件描述符设置为监听状态
  //128表示监听队列的最大长度
  int listenret = listen(sockfd, 128);
  errorCheck(listenret);

  //4.阻塞等待客户端的连接请求
  //accept函数会返回一个新的文件描述符，用于与客户端进行通信
  
  //保存客户端信息到结构体中
  struct sockaddr_in clientaddr;
  memset(&clientaddr, 0, sizeof(clientaddr));
  socklen_t clientaddrlen = sizeof(clientaddr);
  clientaddr.sin_family = AF_INET;
  int communicationfd = accept(sockfd, (struct sockaddr *)&clientaddr, &clientaddrlen);
  errorCheck(communicationfd);

  //打印客户端的IP地址和端口号
  std::cout << "client ip: " << inet_ntoa(clientaddr.sin_addr) << std::endl;
  std::cout << "client port: " << ntohs(clientaddr.sin_port) << std::endl;


  //收发消息
  std::string commbuff;

  while(1)
    {
        char commbuff[128] = {0};
        int len = recv(communicationfd, commbuff, sizeof(commbuff), 0);
        if(len < 0)
        {
            std::cout << "服务器接收数据失败" << std::endl;
        }
        else if(0 == len)
        {
            std::cout << "len == 0" << std::endl;
        }
        else
        {
            std::cout << "recv msg from client : " << commbuff << std::endl;
        }

        std::cout << "我服务器想发送数据给客户端 " << std::endl;
        std::string line;
        getline(std::cin, line);
        int len2 = send(communicationfd, line.data(), line.size(), 0);
        if(len2 < 0)
        {
            std::cout << "服务器发送出错" << std::endl;
        }
        else if(0 == len2)
        {
          std::cout<<"0===len2"<<std::endl;
        }else{
          std::cout<<"发送成功"<<std::endl;
        }
  }
  close(communicationfd);
  close(sockfd);
}
int main(int argc, char *argv[]) { 
  ServerHost(std::string("127.0.0.1"), std::string("8080"));
  return 0; 
}