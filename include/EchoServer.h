/**
 * Project 66th
 */


#ifndef _ECHOSERVER_H
#define _ECHOSERVER_H

#include "TcpServer.h"
#include "ThreadPool.h"

class EchoServer {
public: 

    /**
     * @param threadNum
     * @param queSize
     * @param ip
     * @param port
     */
    EchoServer(size_t threadNum, size_t queSize, const string & ip, unsigned short port);

    ~EchoServer();

    void start();

    void stop();

    /**
     * @param con
     */
    void onNewConnection(const TcpConnectionPtr & con);

    /**
     * @param con
     */
    void onMessage(const TcpConnectionPtr & con);

    /**
     * @param con
     */
    void onClose(const TcpConnectionPtr & con);
private: 
    ThreadPool _pool;
    TcpServer _server;
};

#endif //_ECHOSERVER_H
