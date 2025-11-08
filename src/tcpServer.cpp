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
TcpServer::~TcpServer() = default;

void TcpServer::start()
{
    std::thread t([this]
                  { main_reactor_->eventLoop(); });
    t.detach();
    acceptor->exec();
}