#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <vector>
#include "reactor.h"
#include "acceptorHandler.h"

class TcpServer
{
public:
    TcpServer(const std::string &ip, int port, AllocMode mode, int reactor_size);
    ~TcpServer();
    void start();

private:
    std::shared_ptr<Reactor> main_reactor_;    // 主reactor
    std::shared_ptr<AcceptorHandler> acceptor; // 接受连接处理器
};

#endif