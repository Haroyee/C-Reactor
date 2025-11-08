#include "tcpServer.h"
#include <iostream>

int main()
{
    std::string ip = "0.0.0.0";
    TcpServer server(ip, 8889, AllocMode::LOAD_BASED, 4);
    server.start();
}