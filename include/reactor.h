#ifndef REACTOR_H
#define REACTOR_H
#include <map>
#include <memory>
#include "eventHandler.h"
#include <iostream>
#include <mutex>
// Reactor类
class Reactor
{
public:
    Reactor();

    ~Reactor();
    // 添加事件
    void addHandler(std::shared_ptr<EventHandler> handler, uint32_t events);

    // 修改事件
    void modifyHandler(std::shared_ptr<EventHandler> handler, uint32_t events);

    // 移除事件
    void removeHandler(std::shared_ptr<EventHandler> handler);

    // 事件循环
    void eventLoop();

    int getConnection_count();
    std::map<int, std::shared_ptr<EventHandler>> handlers_;

private:
    int epoll_fd_;
    int connection_count_;
};
#endif