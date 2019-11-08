#include "request.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>

namespace qyutil
{

StreamError TcpSocketStream::open(string ip, short port, int timeout)
{
    StreamError ret = StreamError::ok;
    do {
        hsocket = socket(AF_INET, SOCK_STREAM, 0);
        if(hsocket <= 0)
        {
            ret = sock_invalid;
            break;
        }
        sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(port);
        int code = connect(hsocket, (sockaddr*)(&addr), sizeof(sockaddr));
        if(code)
        {
            ret = connect_error;
            break;
        }
    }while(false);
    
    return ret;
}

StreamError TcpSocketStream::close()
{
    socket_close(hsocket);
    return StreamError::ok;
}

StreamError TcpSocketStream::read(const char *buf, size_t& length, int timeout)
{
    StreamError ret = StreamError::ok;
    length = recv(hsocket, (void*)buf, length, 0);
    if(length < 0)
    {
        ret = StreamError::read_error;
    }
    return ret;
}

StreamError TcpSocketStream::write(const char *buf, size_t& length, int timeout)
{
    StreamError ret = StreamError::ok;
    int code = send(hsocket, buf, length, 0);
    if(0 > code)
    {
        ret = StreamError::send_error;
    }
    return ret;
}

}
