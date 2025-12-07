#include "socket.h"
#include <cstring>
#include <cerrno>
#include <iostream>
#include <vector>

// 发送带长度前缀的消息（解决粘包）
bool sendMessage(int sock_fd, const std::string &msg)
{
    // 1. 发送4字节长度前缀（主机序转网络序）
    uint32_t body_len = htonl(msg.size());
    ssize_t n = send(sock_fd, &body_len, 4, 0);
    if (n != 4)
    {
        std::cerr << "Send header failed: " << strerror(errno) << std::endl;
        return false;
    }

    // 2. 发送消息体
    n = send(sock_fd, msg.c_str(), msg.size(), 0);
    if (n != (ssize_t)msg.size())
    {
        std::cerr << "Send body failed: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

// 客户端读取响应（需按同样格式拆包）
std::string recvMessage(int sock_fd)
{
    // 1. 读取4字节长度前缀
    uint32_t body_len_net;
    ssize_t n = recv(sock_fd, &body_len_net, 4, 0);
    if (n != 4)
    {
        std::cerr << "Recv header failed: " << strerror(errno) << std::endl;
        return "";
    }
    size_t body_len = ntohl(body_len_net);

    // 2. 读取消息体
    std::vector<char> buf(body_len);
    n = recv(sock_fd, buf.data(), body_len, 0);
    if (n != (ssize_t)body_len)
    {
        std::cerr << "Recv body failed: " << strerror(errno) << std::endl;
        return "";
    }

    return std::string(buf.begin(), buf.end());
}

// 客户端主逻辑示例
int main()
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    // ... 连接服务端逻辑 ...
    Socket client_socket(sock_fd);
    client_socket.Connect("127.0.0.1", 8889);
    for (int i = 0; i < 100; i++)
    {
        std::string msg = "Hello, TCP! No more sticky packets!";
        if (sendMessage(sock_fd, msg))
        {
            std::string resp = recvMessage(sock_fd);
            std::cout << "Received response: " << resp << std::endl;
        }
    }

    close(sock_fd);
    return 0;
}