#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H
#include "eventHandler.h"
#include "reactor.h"
#include <iostream>

// 连接处理器
class ConnectionHandler : public EventHandler
{
public:
    ConnectionHandler(int client_fd, std::shared_ptr<Reactor> reactor);

    ~ConnectionHandler();

    void handleRead() override;

    void handleWrite() override;

private:
    std::shared_ptr<Reactor> reactor_;
    std::string write_buffer_;
    bool write_ready_;
};
#endif