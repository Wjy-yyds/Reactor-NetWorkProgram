/**
 * Project 66th
 */


#include "EchoServer.h"
#include "MyTask.h"

/**
 * EchoServer implementation
 */


/**
 * @param threadNum
 * @param queSize
 * @param ip
 * @param port
 */
EchoServer::EchoServer(size_t threadNum, size_t queSize,
                       const string & ip, unsigned short port) 
: _pool(threadNum,queSize)
, _server(ip,port)
{

}

EchoServer::~EchoServer() {

}

/**
 * @return void
 */
void EchoServer::start() {
    _pool.start();

    using namespace std::placeholders;
    _server.setAllCallback(
                           bind(&EchoServer::onNewConnection,this,_1),
                           bind(&EchoServer::onMessage,this,_1),
                           bind(&EchoServer::onClose,this,_1)
                          );

    _server.start();
}

/**
 * @return void
 */
void EchoServer::stop() {
    _pool.stop();
    _server.stop();
}

/**
 * @param con
 * @return void
 */
void EchoServer::onNewConnection(const TcpConnectionPtr & con) {
    cout << con->toString() << " has connected!!! " << endl;
}

/**
 * @param con
 * @return void
 */
void EchoServer::onMessage(const TcpConnectionPtr & con) {
    string msg = con->receive();
    cout << ">> recv msg from client: " << msg << endl;

    MyTask task(msg,con);
    _pool.addTask(bind(&MyTask::process,task));
}

/**
 * @param con
 * @return void
 */
void EchoServer::onClose(const TcpConnectionPtr & con) {
    cout << con->toString() << " has closed!!! " << endl;
}
