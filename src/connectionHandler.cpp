#include "connectionHandler.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <memory>

ConnectionHandler::ConnectionHandler(int client_fd, Reactor &reactor)
    : EventHandler(client_fd), reactor_(reactor), write_ready_(false)
{
}
ConnectionHandler::~ConnectionHandler()
{
    if (fd_ > 0)
        close(fd_);
}
void ConnectionHandler::handleRead()
{
    std::cout << "开始读取" << std::endl;
    char buffer[1024];
    ssize_t n;

    while ((n = read(fd_, buffer, sizeof(buffer))) > 0)
    {
        buffer[n] = '\0';
        write_buffer_ += buffer;
    }

    if (n == 0)
    {
        // 客户端关闭连接
        std::cout << "Connection closed" << std::endl;
        reactor_.removeHandler(shared_from_this());
        return;
    }
    else if (n < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // 读取完毕
            if (!write_buffer_.empty())
            {
                // 准备写入响应
                write_ready_ = true;
                reactor_.modifyHandler(shared_from_this(), EPOLLIN | EPOLLOUT | EPOLLET);
            }
        }
        else
        {
            std::cerr << "Read error" << std::endl;
            reactor_.removeHandler(shared_from_this());
        }
    }
}

void ConnectionHandler::handleWrite()
{
    if (!write_ready_)
    {
        return;
    }

    ssize_t n = write(fd_, write_buffer_.c_str(), write_buffer_.size());
    if (n < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // 稍后再试
            return;
        }
        std::cerr << "Write error" << std::endl;
        reactor_.removeHandler(shared_from_this());
        return;
    }

    // 写入完成
    write_buffer_.clear();
    write_ready_ = false;

    // 修改为只关注读事件
    reactor_.modifyHandler(shared_from_this(), EPOLLIN | EPOLLET);
}