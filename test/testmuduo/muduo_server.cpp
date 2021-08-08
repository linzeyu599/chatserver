/*
muduo网络库给用户提供了两个主要的类
TcpServer： 用于编写服务器程序的
TcpClient： 用于编写客户端程序的

epoll + 线程池
好处：能够把网络I/O的代码和业务代码区分开
业务代码就是：用户的连接和断开，用户的可读写事件
我们只需要关注业务代码，什么时候发生和如何监听这些事情的发生由muduo库去做 
*/
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>//事件循环 
#include <iostream>
#include <functional>//绑定器 
#include <string>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;
//muduo的名字空间作用域
 
/*
基于muduo网络库开发服务器程序
步骤如下： 
1.组合TcpServer对象
2.创建EventLoop事件循环对象的指针
3.明确TcpServer构造函数需要什么参数，输出ChatServer的构造函数
4.在当前服务器类的构造函数当中，注册处理连接的回调函数和处理读写时间的回调函数
5.设置合适的服务端线程数量，muduo库会自己分配I/O线程和worker线程
*/
class ChatServer//TCPServer 
{
public:
    ChatServer(EventLoop *loop,               //事件循环
               const InetAddress &listenAddr, //muduo封装好的，绑定IP+Port
               const string &nameArg)//给TCPserver一个名字 
        : _server(loop, listenAddr, nameArg), _loop(loop)//没有默认构造哦 
    {
        //给服务器注册用户连接的创建和断开的回调，回调就是对端的相应事件发生了告诉网络库 ，然后网络库告诉我 ，我在回调函数开发业务 
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));//绑定this对象到这个方法中，_1是参数占位符 

        //给服务器注册用户读写事件的回调
        _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));//绑定this对象到这个方法中 

        //设置服务器端的线程数量 1个I/O线程（监听新用户的连接事件）， 3个worker线程
        //不设置的话，就1个线程而已，要处理连接又要处理业务 
        _server.setThreadNum(4);//设置4个线程，1个I/O线程，3个worker线程 
    }

    void start()//开启事件循环 
    {
        _server.start();
    }

private:
    //专门处理：用户的连接创建和断开 epoll listenfd accept
    //如果有新用户的连接或者断开，muduo库就会调用这个函数 
    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected())//连接 ， peerAddress()对端的地址 localAddress() 本地的地址 
        {
            cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " state:online" << endl;
        }
        else//断开 
        {
            cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " state:offline" << endl;
            conn->shutdown();//相当于这些close(fd)
            //_loop->quit();
        }
    }

    //专门处理：用户的读写事件，muduo库去调用这个函数 
    void onMessage(const TcpConnectionPtr &conn, //连接，通过这个连接可以读写数据 
                   Buffer *buffer,               //缓冲区，提高数据收发的性能 
                   Timestamp time)               //接收到数据的时间信息
    {
        string buf = buffer->retrieveAllAsString();//收到的数据放到这个字符串中 
        cout << "recv data:" << buf << " time:" << time.toFormattedString() << endl;
        conn->send(buf);//返回 ，收到什么发送什么 
    }

    TcpServer _server;//第一步 
    EventLoop *_loop; //第二步相当于 epoll 事件循环的指针，有事件发生，loop上报 
};

int main()
{
    EventLoop loop;//相当于像是创建了epoll
    InetAddress addr("127.0.0.1", 6000);//IP地址，端口号 
    ChatServer server(&loop, addr, "ChatServer");

    server.start();//listenfd通过 epoll_ctl 添加到 epoll 
    loop.loop();//相当于epoll_wait，以阻塞方式等待新用户连接，已连接用户的读写事件等
}
