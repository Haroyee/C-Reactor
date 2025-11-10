#ifndef ACCEPTORHANDLER_H
#define ACCEPTORHANDLER_H
#include "eventHandler.h"
#include "reactor.h"
#include "socket.h"
#include <vector>
#include <thread>
#include <atomic>
#include <memory>

// 客户端fd分配模式
enum class AllocMode
{
    ROUND_ROBIN, // 轮询
    LOAD_BASED,  // 负载均衡
};

// 接受连接处理器
class AcceptorHandler : public EventHandler
{
public:
    AcceptorHandler(const std::string &ip, int port, int reactor_size, AllocMode mode);

    ~AcceptorHandler();

    void handleRead() override;

    void handleWrite() override;

    void exec();

private:
    void gen_index(); // 生成下一个reactor的index

    Socket server;
    AllocMode mode_;                                    // 客户端fd分配模式
    std::atomic<int> next_index_;                       // 轮询计算下一个分配index
    std::vector<std::shared_ptr<Reactor>> sub_reactor_; // 从reactor数组
    std::vector<std::shared_ptr<std::thread>> threads;
};
#endif