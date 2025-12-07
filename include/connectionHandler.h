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
    // 核心：处理接收缓冲区，拆包逻辑
    void processRecvBuffer();
    // 辅助：处理单条完整消息（业务逻辑入口）
    void handleCompleteMessage(const std::string &msg);

    std::shared_ptr<Reactor> reactor_;
    std::string write_buffer_;
    bool write_ready_;
    std::vector<char> recv_buffer_; // 接收缓冲区（存未拆完的字节）
    size_t expected_body_len_;      // 期望读取的消息体长度
    bool is_reading_header_;        // 状态：是否正在读取消息头（4字节长度）
};
#endif