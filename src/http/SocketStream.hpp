#ifndef __SOCKETSTREAM_HPP__
#define __SOCKETSTREAM_HPP__

#include <errno.h>
#include "net_common/net_common.h"

using namespace NetCommon;

enum StreamError : int
{
    ok = 0,
    sock_invalid,
    connect_timeout,
    send_timeout,
    read_timeout,
    connect_error,
    send_error,
    read_error,
};

class TcpSocketStream
{
public:
    virtual StreamError open(string ip, short port, double timeout)
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
            
            timeval timeout_ = dtime2timeval(timeout);

            setsockopt(hsocket, SOL_SOCKET, SO_RCVTIMEO, &timeout_, sizeof(timeout_));
            setsockopt(hsocket, SOL_SOCKET, SO_RCVTIMEO, &timeout_, sizeof(timeout_));
            
            int code = connect(hsocket, (sockaddr*)(&addr), sizeof(sockaddr));
            if(code)
            {
                ret = connect_error;
                break;
            }
        }while(false);
        
        return ret;
    }
    virtual StreamError close()
    {
        socket_close(hsocket);
        return StreamError::ok;
    }
    virtual StreamError read(const char *buf, size_t& length, int timeout)
    {
        StreamError ret = StreamError::ok;
        length = recv(hsocket, (void*)buf, length, 0);
        if(length < 0)
        {
            ret = StreamError::read_error;
        }
        return ret;
    }
    virtual StreamError write(const char *buf, size_t& length, int timeout)
    {
        StreamError ret = StreamError::ok;
        int code = send(hsocket, buf, length, 0);
        if(0 > code)
        {
            ret = StreamError::send_error;
        }
        return ret;
    }

    private:
        RawSocket hsocket;
};

#endif
