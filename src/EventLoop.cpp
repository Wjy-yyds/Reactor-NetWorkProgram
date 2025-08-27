/**
 * Project 66th
 */


#include "EventLoop.h"
#include <sys/eventfd.h>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

/**
 * EventLoop implementation
 */


/**
 * @param acc
 */
EventLoop::EventLoop(Acceptor & acc) 
    : _epfd(createEpollFd())
    , _evlist(1024)
    , _isLooping(false)
    , _acceptor(acc)
    , _conns()
    , _evtfd(createEventfd())
    , _pendings()
    , _mutex()
{
    //因为数据成员vector是在epoll_wait函数中充当就绪列表
    //所以一定预留存放元素的空间
    //如果是一个空的vector，那么也至少应该reverse


    //需要将listenfd加到红黑树上进行监听
    //要listenfd，需要Acceptor中添加一个函数来获取
    int listenfd = _acceptor.fd();
    addEpollReadFd(listenfd);

    //也要监听用于通知的文件描述符
    addEpollReadFd(_evtfd);
}

EventLoop::~EventLoop() {
    close(_epfd);
}

/**
 * @return void
 */
void EventLoop::loop() {
    _isLooping = true;
    while(_isLooping)
    {
        waitEpollFd();
    }
}

/**
 * @return void
 */
void EventLoop::unloop() {
    _isLooping = false;
}

/**
 * @return int
 */
int EventLoop::createEpollFd() {
    int fd = epoll_create(100);
    if(fd == -1)
    {
        perror("createEpollFd");
        return -1;
    }
    return fd;
}

/**
 * @return void
 */
void EventLoop::addEpollReadFd(int fd) {
    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = fd;

    int ret = epoll_ctl(_epfd,EPOLL_CTL_ADD,fd,&evt);
    if(-1 == ret)
    {
        perror("addEpollReadFd");
        return;
    }
}

/**
 * @return void
 */
void EventLoop::delEpollReadFd(int fd) {
    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = fd;

    int ret = epoll_ctl(_epfd,EPOLL_CTL_DEL,fd,&evt);
    if(-1 == ret)
    {
        perror("delEpollReadFd");
        return;
    }
}

/**
 * @return void
 */
void EventLoop::waitEpollFd() {
    int nready = 0;
    do{
        //vector充当就绪列表，
        //其实是需要拿存放元素的空间来存放epoll_event
        //这里要考虑如何获取vector存放元素的空间的首地址
        nready = epoll_wait(_epfd,_evlist.data(),_evlist.size(),3000);
    }while(-1 == nready && errno == EINTR);

    if(-1 == nready)
    {
        cerr << "-1 == nready" << endl;
        return;
    }
    else if(0 == nready)
    {
        cout << ">> epoll_wait timeout!!!" << endl;
    }
    else
    {
        //如果nready已经到达vector的容量上限
        //那么后续就绪的文件描述很可能就会超过vector的容量
        //那么需要进行扩容
        //但是自动扩容的途径如push_back/insert等
        //不可能在epoll_wait中进行调用，
        //所以只能手动扩容
        if(nready == (int)_evlist.size())
        {
            //如果采用push_back，确实可以扩容
            //但是扩容之后，size实际上等于1025
            //而epoll_wait没有改变vector的size的能力
            _evlist.resize(2 * nready);
        }

        for(int idx = 0; idx < nready; ++idx)
        {
            int listenfd = _acceptor.fd();
            int fd = _evlist[idx].data.fd;

            if(fd == listenfd) //有新的连接
            {
                handleNewConnection();
            }
            else if(fd == _evtfd)
            {
                //针对eventfd进行读操作，
                //也就是将计数器清空
                handleRead();

                //收到通知之后，执行任务
                doPendingFunctors();
            }
            else
            {
                //老的连接有数据传入
                handleMessage(fd);
            }
        }
    }
}

/**
 * @return void
 */
void EventLoop::handleNewConnection() {
    //只要accept函数执行成功，表明三次握手建立成功
    //从全连接队列中取出一条新的连接
    int connfd = _acceptor.accept();
    if(-1 == connfd)
    {
        perror("handleNewConnection");
        return;
    }

    //connfd放到红黑树上进行监听
    addEpollReadFd(connfd);

    //利用代表连接的文件描述符connfd创建堆上的TcpConnection
    //交给shared_ptr管理
    TcpConnectionPtr con(new TcpConnection(connfd,this));

    //创建完TcpConnection之后，它内部的function是空的
    //所以需要由EventLoop再做一次转交，让TcpConnection的function
    //也能关联外部的函数
    con->setNewConnectionCallback(_onNewConnection);
    con->setMessageCallback(_onMessage);
    con->setCloseCallback(_onClose);


    /* _conns.insert({connfd,con}); */
    _conns[connfd] = con;

    //新连接建立的回调执行
    con->handleNewConnectionCallback();
}

/**
 * @param fd
 * @return void
 */
void EventLoop::handleMessage(int fd) {
    auto it = _conns.find(fd);
    if(it != _conns.end())
    {
        //先判断连接是否已经断开了
        bool flag = it->second->isClosed();

        if(flag)
        {
            //连接断开情况的回调
            it->second->handleCloseCallback();

            //如果连接断开了，那么就要
            //从红黑树上删除fd，取消监听
            //以及从map中删除对应的记录
            delEpollReadFd(fd);
            _conns.erase(it);
        }
        else
        {
            //消息到达情况的回调
            it->second->handleMessageCallback();          
        }
    }
    else
    {
        cout << "连接不存在" << endl;
        return;
    }
}


void EventLoop::setNewConnectionCallback(TcpConnectionCallback && cb)
{
    _onNewConnection = std::move(cb);
}
void EventLoop::setMessageCallback(TcpConnectionCallback && cb)
{
    _onMessage = std::move(cb);
}
void EventLoop::setCloseCallback(TcpConnectionCallback && cb)
{
    _onClose = std::move(cb);
}


int EventLoop::createEventfd()
{
    int fd = ::eventfd(0,0);
    if(fd == -1)
    {
        perror("createEventfd");
        return -1;
    }
    return fd;
}

void EventLoop::handleRead()
{
    uint64_t val = 1;
    ssize_t ret = read(_evtfd,&val,sizeof(uint64_t));
    if(ret != sizeof(uint64_t))
    {
        perror("handleRead");
        return;
    }
}
void EventLoop::wakeup()
{
    uint64_t val = 1;
    ssize_t ret = write(_evtfd,&val,sizeof(uint64_t));
    if(ret != sizeof(uint64_t))
    {
        perror("wakeup");
        return;
    }
}

void EventLoop::runInLoop(Functor && cb)
{
    {
        lock_guard<mutex> lg(_mutex);
        _pendings.push_back(std::move(cb));
    }

    //向计数器中写入数值
    //主线程监听着eventfd，发现计数器中的值不为0，
    //即代表eventfd就绪，相应地去执行任务
    wakeup();
}


void EventLoop::doPendingFunctors()
{
    //针对EventLoop中的任务队列
    //就类似于生产者消费者模型中的数据仓库
    //生产者（子线程）向仓库中添加任务，是持有锁的
    //消费者（主线程）从仓库中取走任务，也是持有锁的
    vector<Functor> temp;
    {
        lock_guard<mutex> lg(_mutex);
        temp.swap(_pendings);//一次性取走所有任务
    }

    //主线程自己慢慢执行
    for(auto & cb : temp)
    {
        cb();
    }
}
