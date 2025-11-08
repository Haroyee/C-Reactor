#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h> // socket()、bind()等核心函数
#include <netinet/in.h> // sockaddr_in地址结构
#include <arpa/inet.h>  // inet_pton()、htons()等转换函数
#include <unistd.h>     // close()、read()、write()
#include <string>

class Socket
{
private:
    int m_sockfd;
    struct sockaddr_in m_addr;

public:
    Socket();

    Socket(int);

    ~Socket();

    bool Bind(const std::string &, int);

    bool Connect(const std::string &, int);

    bool Listen(int);

    int Accept();

    int Send(const char *, int);

    int Recv(char *, int);

    void Close();
};

#endif
