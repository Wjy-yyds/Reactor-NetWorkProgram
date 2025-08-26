#include "TcpServer.h"
#include "EventLoop.h"
#include "ThreadPool.h"
#include <iostream>
using std::cout;
using std::endl;

class MyTask
{
public:
    MyTask(const string & msg,const TcpConnectionPtr & con)
    : _msg(msg)
    , _con(con)
    {}
        
    void process()
    {
        //如何处理msg
        _msg = "msg: " + _msg;

        //处理完之后进行发送，是要发送给主线程
        //如果直接在这里进行send，那么就是
        //由子线程发送数据给客户端，与设计的目的不符
        /* _con->send(_msg); */



        //在TcpConnection中增加一个成员函数
        //能够让子线程将处理完的数据发送给主线程
        //还需要一个手段实现这件事情 —— eventfd
        //
        //问题1：主线程何时接收，应该由子线程在处理完数据进行提醒
        //提醒的方式就是eventfd
        //主线程可以监听eventfd（计数器的值初始化为0）
        //子线程处理完数据，就对eventfd进行写操作，
        //主线程发现计数器的值不为0了，就相当于收到了通知，可以接收
        //
        //问题2: 子线程如何发送数据给主线程
        //通过共享资源 —— 其实这里的发送和主线程发送任务给子线程高度一致
        //
        //主线程之前将要处理的数据msg、如何处理数据process、连接信息con
        //打包成function交给了线程池，线程池中有一个任务队列存放function
        //子线程在合适的时机从任务队列中取任务执行
        //
        //
        //那么反过来，Reactor这边也可以准备一个任务队列
        //子线程处理完数据，将处理后的数据msg、要进行发送的函数send、连接信息con
        //打包成function，交给Reactor的任务队列
        //再通过eventfd进行通知，主线程收到通知后，
        //从自己的任务队列中取任务执行
        //
        con->sendInLoop(_msg);//发送给主线程，而不是发送给客户端
    }

private:
    string _msg;
    TcpConnectionPtr _con;
};

//连接建立
void onNewConnection(const TcpConnectionPtr & con)
{
    cout << con->toString() << " has connected!!! " << endl;
}


//消息到达
void onMessage(const TcpConnectionPtr & con,ThreadPool & pool)
{
    //基于这条连接接收客户端发来的数据
    string msg = con->receive();
    cout << ">> recv msg from client: " << msg << endl;

    //在客户端发送数据时，主线程最终会调用onMessage函数
    //考虑到处理数据的过程可能耗时较多
    //所以将处理数据的工作交给线程池中的子线程进行
    //需要将msg，以及设定如何处理的函数process，
    //以及能够进行发送数据的TcpConnectionPtr
    //打包成function
    //然后加到线程池的任务队列中
    MyTask task(msg,con);
    pool.addTask(bind(&MyTask::process,task));


    //将处理过后的数据发回客户端
    con->send(msg);
}

//连接断开
void onClose(const TcpConnectionPtr & con)
{
    cout << con->toString() << " has closed!!! " << endl;
}


#if 0
void test0(){
    Acceptor acceptor("127.0.0.1",8888);
    acceptor.ready();

    EventLoop eloop(acceptor);
    
    eloop.setNewConnectionCallback(onNewConnection);
    eloop.setMessageCallback(onMessage);
    eloop.setCloseCallback(onClose);

    eloop.loop();
}
#endif

void test1()
{
    TcpServer server("127.0.0.1",8888);
    server.setAllCallback(onNewConnection,
                          onMessage,
                          onClose);

    server.start();
}

int main(void){
    test1();
    return 0;
}

