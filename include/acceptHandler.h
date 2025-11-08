#ifndef ACCEPTHANDLER_H
#define ACCEPTHANDLER_H
#include "eventHandler.h"
#include "reactor.h"
// 接受连接处理器
class AcceptorHandler : public EventHandler
{
public:
    AcceptorHandler(int port, Reactor &reactor);

    ~AcceptorHandler();

    void handleRead() override;

    void handleWrite() override;

private:
    Reactor &reactor_;
};
#endif