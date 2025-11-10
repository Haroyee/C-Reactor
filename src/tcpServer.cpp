#include "tcpServer.h"
#include "acceptorHandler.h"
#include "connectionHandler.h"
#include <iostream>
#include <sys/epoll.h>
#include <thread>

TcpServer::TcpServer(const std::string &ip, int port, AllocMode mode = AllocMode::ROUND_ROBIN, int reactor_size = 4)
{
    acceptor = std::make_shared<AcceptorHandler>(ip, port, reactor_size, mode);

    main_reactor_ = std::make_shared<Reactor>();

    main_reactor_->addHandler(acceptor, EPOLLIN);
}
TcpServer::~TcpServer()
{
    if (t->joinable())
        t->join();
};

void TcpServer::start()
{
    acceptor->exec();
    t = std::make_shared<std::thread>([this]
                                      { main_reactor_->eventLoop(); });
}