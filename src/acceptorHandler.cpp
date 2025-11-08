#include "acceptorHandler.h"
#include "connectionHandler.h"
#include "socket.h"
#include "tcpServer.h"
#include <sys/epoll.h>
#include <thread>
AcceptorHandler::AcceptorHandler(const std::string &ip, int port, int reactor_size, AllocMode mode)
    : EventHandler(-1), mode_(mode), next_index_(0)
{
    fd_ = server.getFd();
    server.Bind(ip, port);
    if (!server.Listen(1024))
        exit(EXIT_FAILURE);
    for (int i = 0; i < reactor_size; ++i)
    {
        sub_reactor_.emplace_back();
    }
}

AcceptorHandler::~AcceptorHandler()
{
    if (fd_ > 0)
    {
        close(fd_);
    }
}

void AcceptorHandler::handleRead()
{
    int client_fd = server.Accept();
    if (client_fd < 0)
        return;
    auto handler = std::make_shared<ConnectionHandler>(client_fd, sub_reactor_[next_index_]);
    sub_reactor_[next_index_].addHandler(handler, EPOLLIN | EPOLLET);
    gen_index();
}
// 作为接受器，不需要处理写事件
void AcceptorHandler::handleWrite() {};

void AcceptorHandler::exec()
{
    for (auto &it : sub_reactor_)
    {
        std::thread t([&]
                      { it.eventLoop(); });
        t.detach();
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
            if (sub_reactor_[i].getConnection_count() < sub_reactor_[next_index_].getConnection_count())
                next_index_ = i;
        }
    }
}