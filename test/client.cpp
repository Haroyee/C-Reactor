#include "socket.h"
#include <iostream>
#include <string>
#include <cstring>
int main()
{

    Socket c_socket;

    if (c_socket.Connect("127.0.0.1", 8889))
    {
        std::string sbuf;
        char rbuf[1024];
        for (int i = 0; i < 10000; ++i)
        {
            sbuf.clear();
            sbuf = "helloooooooooooooooooooooooooooooooooooooooooooooooooooo";
            if (c_socket.Send(sbuf.c_str(), sbuf.size()))
            {
                memset(rbuf, 0, sizeof(rbuf));
                c_socket.Recv(rbuf, sizeof(rbuf));
            }
        }
        c_socket.Close();
    }

    return 0;
}
