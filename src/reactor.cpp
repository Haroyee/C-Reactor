#include "reactor.h"
#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>

const int MAX_EVENTS = 1024;

Reactor::Reactor() : connection_count_(0)
{
    // 创建epoll实例
    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ == -1)
    {
        std::cerr << "Failed to create epoll instance" << std::endl;
        exit(EXIT_FAILURE);
    }
}
Reactor::~Reactor()
{
    if (epoll_fd_ != -1)
    {
        close(epoll_fd_);
    }
}
void Reactor::addHandler(std::shared_ptr<EventHandler> handler, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = handler->getFd();

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, handler->getFd(), &ev) == -1)
    {
        std::cerr << "Failed to add fd to epoll" << std::endl;
        return;
    }

    handlers_[handler->getFd()] = handler;

    connection_count_++;
}
void Reactor::modifyHandler(std::shared_ptr<EventHandler> handler, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = handler->getFd();

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, handler->getFd(), &ev) == -1)
    {
        std::cerr << "Failed to modify fd in epoll" << std::endl;
        return;
    }
}

void Reactor::removeHandler(std::shared_ptr<EventHandler> handler)
{
    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, handler->getFd(), nullptr);
    handlers_.erase(handler->getFd());
    connection_count_--;
}

void Reactor::eventLoop()
{
    struct epoll_event events[MAX_EVENTS];

    while (true)
    {
        int nfds = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            std::cerr << "epoll_wait error " << std::endl; // 输出具体错误信息
            if (errno == EINTR)
                continue; // 忽略信号中断，继续等待
            break;
        }

        for (int i = 0; i < nfds; i++)
        {

            int fd = events[i].data.fd;
            auto it = handlers_.find(fd);
            if (it != handlers_.end())
            {

                if (events[i].events & EPOLLIN)
                {
                    it->second->handleRead();
                }

                if (events[i].events & EPOLLOUT)
                {

                    it->second->handleWrite();
                }
            }
        }
    }
}

int Reactor::getConnection_count()
{
    return connection_count_;
}