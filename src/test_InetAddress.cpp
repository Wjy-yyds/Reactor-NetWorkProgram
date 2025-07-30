#include "EventLoop.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include "NonCopyable.h"
#include "Socket.h"
#include "SocketIO.h"
#include "TcpConnection.h"
#include <functional>
#include <memory>
#include <iostream>
#include <sys/socket.h>

using std::cout;
using std::endl;
void onMessage(const TcpConnectionPtr & conn){
    std::cout<<"onMessage   use  ..."<<std::endl;
        std::string msg = conn->receive();
        std::cout<<"recv msg from client:"<<msg<<std::endl;
        // if(msg==""){
        //     return;
        // }else{
        //     conn->send(msg);
        // }
        conn->send(msg);
        
}

void onNewConnection(const TcpConnectionPtr & conn){
    std::cout<<"onNewConnection   use ..."<<std::endl;
    std::cout<<conn->toString()<<" has connected!"<<std::endl;
}

void onClose(const TcpConnectionPtr & conn){
    std::cout<<"onClose   use..."<<std::endl;
    std::cout<<conn->toString()<<" has closed!"<<std::endl;
}


int main() {
    
    Acceptor acceptor("127.0.0.1",8888);
    acceptor.ready();
    EventLoop loop(acceptor);
    loop.setOnMessageCallback(std::bind(onMessage,std::placeholders::_1));
    loop.setOnNewConnectionCallback(std::bind(onNewConnection,std::placeholders::_1));
    loop.setOnCloseCallback(std::bind(onClose,std::placeholders::_1));
    loop.loop();
    //loop.unloop();

   
    //loop.unloop();
    return 0;
}
