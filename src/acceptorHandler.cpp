#include "acceptorHandler.h"
#include "connectionHandler.h"
#include "socket.h"
#include "tcpServer.h"
#include <sys/epoll.h>
#include <thread>
#include <fcntl.h>

// 工具函数，设置socket为非阻塞
void setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
AcceptorHandler::AcceptorHandler(const std::string &ip, int port, int reactor_size, AllocMode mode)
    : EventHandler(-1), mode_(mode), next_index_(0)
{
    fd_ = server.getFd();
    server.Bind(ip, port);
    if (!server.Listen(1024))
        exit(EXIT_FAILURE);
    for (int i = 0; i < reactor_size; ++i)
    {
        sub_reactor_.emplace_back(std::make_shared<Reactor>());
    }
}

AcceptorHandler::~AcceptorHandler()
{
    for (auto &it : threads)
    {
        if (it->joinable())
            it->join();
    }
    if (fd_ > 0)
    {
        close(fd_);
    }
}

void AcceptorHandler::handleRead()
{
    while (true)
    {
        int client_fd = server.Accept();
        if (client_fd < 0)
        {
            // EAGAIN 或 EWOULDBLOCK 说明已经处理了所有连接
            break;
        }
        setNonBlocking(client_fd);

        auto handler = std::make_shared<ConnectionHandler>(client_fd, sub_reactor_[next_index_]);
        sub_reactor_[next_index_]->addHandler(handler, EPOLLIN | EPOLLET);

        gen_index();
    }
}
// 作为接受器，不需要处理写事件
void AcceptorHandler::handleWrite() {};

void AcceptorHandler::exec()
{
    for (auto &reactor_ptr : sub_reactor_)
    {
        threads.emplace_back(std::make_shared<std::thread>([reactor_ptr]
                                                           { reactor_ptr->eventLoop(); }));
    }
}

void AcceptorHandler::gen_index()
{
    if (mode_ == AllocMode::ROUND_ROBIN)
    {
        next_index_ = (next_index_ + 1) % sub_reactor_.size();
    }

    else if (mode_ == AllocMode::LOAD_BASED)
    {
        next_index_ = 0;
        for (size_t i = 0; i < sub_reactor_.size(); ++i)
        {
            if (sub_reactor_[i]->getConnection_count() < sub_reactor_[next_index_]->getConnection_count())
                next_index_ = i;
        }
    }
}