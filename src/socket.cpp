#include "socket.h"
#include <iostream>
#include <cstring>

Socket::Socket()
{
    m_sockfd_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_sockfd_ == -1)
    {
        std::cerr << "create socket failed" << std::endl;
        return;
    }
    memset(&m_addr_, 0, sizeof(m_addr_));
    std::cout << "socker created!" << std::endl;
}

Socket::Socket(int sockfd) : m_sockfd_(sockfd)
{

    memset(&m_addr_, 0, sizeof(m_addr_));
}

Socket::~Socket()
{
    Close();
}

bool Socket::Bind(const std::string &ip, int port)
{
    // 允许地址复用
    int opt = 1;
    setsockopt(m_sockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    m_addr_.sin_family = AF_INET;   // IPv4 协议族
    m_addr_.sin_port = htons(port); // 端口号（需转为网络字节序）
    if (ip.empty())
        m_addr_.sin_addr.s_addr = htonl(INADDR_ANY); // 若没输入则绑定本机所有ip
    else
        inet_pton(AF_INET, ip.c_str(), &m_addr_.sin_addr.s_addr); // 绑定指定ip
    if (bind(m_sockfd_, (struct sockaddr *)&m_addr_, sizeof(m_addr_)) == -1)
    {
        perror("bind failed");
        return false;
    }
    else
    {
        return true;
    }
}

bool Socket::Listen(int backlog)
{
    if (listen(m_sockfd_, backlog) == -1)
    {
        perror("listen failed");
        return false;
    }
    else
    {
        std::cout << "socket listen ......." << std::endl;
        return true;
    }
}

int Socket::Accept()
{
    struct sockaddr_in c_addr;
    unsigned int addr_len = sizeof(c_addr);
    // 设置socket为非阻塞
    int connfd = accept4(m_sockfd_, (struct sockaddr *)&c_addr, &addr_len, SOCK_NONBLOCK);
    if (connfd == -1)
    {
        perror("accept failed");
        return -1;
    }
    else
    {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &c_addr.sin_addr.s_addr, ip, INET_ADDRSTRLEN);
        std::cout << "client:" << ip << ":" << ntohs(c_addr.sin_port) << "link successful!" << std::endl;
        return connfd;
    }
}

bool Socket::Connect(const std::string &ip, int port)
{
    m_addr_.sin_family = AF_INET;                             // IPv4 协议族
    m_addr_.sin_port = htons(port);                           // 端口号（需转为网络字节序）
    inet_pton(AF_INET, ip.c_str(), &m_addr_.sin_addr.s_addr); // 绑定指定ip

    if (connect(m_sockfd_, (struct sockaddr *)&m_addr_, sizeof(m_addr_)) == -1)
    {
        perror("connect failed");
        return false;
    }
    else
    {
        std::cout << "server:" << ip << ":" << port << "link successful!" << std::endl;
        return true;
    }
}

int Socket::Send(const char *buf, int len)
{
    return send(m_sockfd_, buf, len, 0);
}

int Socket::Recv(char *buf, int len)
{
    return recv(m_sockfd_, buf, len, 0);
}

void Socket::Close()
{
    if (m_sockfd_ > 0)
        close(m_sockfd_);
}

int Socket::getFd()
{
    return m_sockfd_;
}