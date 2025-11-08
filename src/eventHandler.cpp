#include "eventHandler.h"

// 事件处理器基类
EventHandler::EventHandler(int fd) : fd_(fd)
{
}
int EventHandler::getFd() const
{
    return fd_;
}