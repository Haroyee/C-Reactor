#include "connectionHandler.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <memory>
#include <arpa/inet.h> // inet_pton()、htons()等转换函数
#include <cstring>     //memcpy

ConnectionHandler::ConnectionHandler(int client_fd, std::shared_ptr<Reactor> reactor)
    : EventHandler(client_fd), reactor_(reactor), write_ready_(false),
      expected_body_len_(0), is_reading_header_(true)
{
    recv_buffer_.reserve(8192); // 预分配缓冲区，减少内存碎片
}

ConnectionHandler::~ConnectionHandler()
{
    if (fd_ > 0)
        close(fd_);
}
void ConnectionHandler::handleRead()
{
    char temp_buf[4096];
    ssize_t n = 0;

    // 非阻塞读取所有可用数据（ET模式需一次性读完）
    while ((n = read(fd_, temp_buf, sizeof(temp_buf))) > 0)
    {
        // 把读取到的字节追加到接收缓冲区
        recv_buffer_.insert(recv_buffer_.end(), temp_buf, temp_buf + n);
        // 处理缓冲区：尝试拆出完整消息
        processRecvBuffer();
    }
    // std::cout << "client " << fd_ << ": " << write_buffer_ << std::endl;
    if (n == 0)
    {
        // 客户端关闭连接
        // std::cout << "Connection closed" << std::endl;
        reactor_->removeHandler(shared_from_this());
        return;
    }
    else if (n < 0)
    {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            // 非超时错误，关闭连接
            std::cerr << "Read error (fd=" << fd_ << "): " << strerror(errno) << std::endl;
            reactor_->removeHandler(shared_from_this());
        }
        // EAGAIN/EWOULDBLOCK：无数据可读，直接返回
        return;
    }
}

void ConnectionHandler::handleWrite()
{
    if (!write_ready_ || write_buffer_.empty())
    {
        return;
    }

    // 第一步：发送4字节长度前缀（主机序 -> 网络序）
    uint32_t body_len_net = htonl(write_buffer_.size());
    ssize_t n = write(fd_, &body_len_net, 4);
    if (n != 4)
    {
        if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            std::cerr << "Write error (fd=" << fd_ << "): " << strerror(errno) << std::endl;
            reactor_->removeHandler(shared_from_this());
        }
        return;
    }

    // 第二步：发送消息体
    n = write(fd_, write_buffer_.c_str(), write_buffer_.size());
    if (n < 0)
    {
        std::cerr << "Error event (fd=" << fd_ << ")" << std::endl;
        reactor_->removeHandler(shared_from_this());
        return;
    }

    // 发送完成，清理缓冲区 + 注销写事件
    write_buffer_.clear();
    write_ready_ = false;
    reactor_->modifyHandler(shared_from_this(), EPOLLIN | EPOLLET);
    std::cout << "Message sent to client (fd=" << fd_ << ")" << std::endl;
}

void ConnectionHandler::processRecvBuffer()
{
    while (true)
    {
        if (is_reading_header_)
        {
            // 第一步：读取4字节消息头（长度前缀）
            if (recv_buffer_.size() < 4)
            {
                // 头部未读全，退出等待下次数据
                break;
            }

            // 解析长度（网络序 -> 主机序）
            uint32_t body_len_net;
            memcpy(&body_len_net, recv_buffer_.data(), 4);
            expected_body_len_ = ntohl(body_len_net);

            // 移除已解析的头部，切换到读取消息体状态
            recv_buffer_.erase(recv_buffer_.begin(), recv_buffer_.begin() + 4);
            is_reading_header_ = false;

            // 防御：消息体长度异常（避免恶意数据）
            if (expected_body_len_ > 1024 * 1024)
            { // 限制最大1MB
                std::cerr << "Invalid message length: " << expected_body_len_ << std::endl;
                reactor_->removeHandler(shared_from_this());
                return;
            }
        }
        else
        {
            // 第二步：读取消息体（按解析出的长度读取）
            if (recv_buffer_.size() < expected_body_len_)
            {
                // 消息体未读全，退出等待下次数据
                break;
            }

            // 提取完整消息体
            std::string msg(recv_buffer_.begin(), recv_buffer_.begin() + expected_body_len_);
            // 处理业务逻辑
            handleCompleteMessage(msg);

            // 移除已处理的消息体，重置状态为读取下一个消息头
            recv_buffer_.erase(recv_buffer_.begin(), recv_buffer_.begin() + expected_body_len_);
            is_reading_header_ = true;
        }
    }
}

void ConnectionHandler::handleCompleteMessage(const std::string &msg)
{
    std::cout << "Received complete message (fd=" << fd_ << "): " << msg << std::endl;
    // 回显给客户端（先存入发送缓冲区，由写事件触发发送）
    write_buffer_ += msg;
    // 注册写事件（ET模式需主动触发）
    if (!write_ready_)
    {
        write_ready_ = true;
        reactor_->modifyHandler(shared_from_this(), EPOLLIN | EPOLLOUT | EPOLLET);
    }
}