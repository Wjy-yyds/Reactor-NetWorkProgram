#ifndef __MyTask_H__
#define __MyTask_H__


#include "TcpServer.h"

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
        _con->sendInLoop(_msg);//发送给主线程，而不是发送给客户端
    }

private:
    string _msg;
    TcpConnectionPtr _con;
};



#endif

