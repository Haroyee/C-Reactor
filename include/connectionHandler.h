#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H
#include "eventHandler.h"
#include "reactor.h"
#include <iostream>
#include <vector>

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
    std::vector<char> recvBuffer_; // 接收缓冲区，用于存储未处理完的数据
    size_t expectedLength_;        // 期望接收的下一个消息体的长度（不包括长度前缀）
};
#endif