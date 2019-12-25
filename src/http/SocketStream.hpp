#ifndef __SOCKETSTREAM_HPP__
#define __SOCKETSTREAM_HPP__

#include <errno.h>
#include "net_common/net_common.h"

using namespace NetCommon;

namespace qyutil
{

enum StreamError : int
{
    ok = 0,
    sock_invalid,
    connect_timeout,
    send_timeout,
    read_timeout,
    connect_error,
    send_error,
    read_error = -2,
};

const size_t ReadBufLength = 1024;

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
    virtual  size_t read(const char *buf, size_t length, int timeout)
    {
        return recv(hsocket, (void*)buf, length, 0);
    }
    virtual size_t write(const char *buf, size_t length, int timeout)
    {
        return send(hsocket, buf, length, 0);
    }

    private:
        RawSocket hsocket;
};

class BaseResponse
{
public:
    BaseResponse() : m_header(ReadBufLength, 0)
    {
        m_buf = new char[ReadBufLength];
        m_boday = new char[ReadBufLength];
    }
    ~BaseResponse()
    {
        delete [] m_buf;
    }
    int get_header()
    {
        size_t eof_pos = string::npos;
        do
        {
            memset(m_buf, 0, ReadBufLength);
            size_t cur_read_count = m_socket_stream->read(m_buf, ReadBufLength, m_timeout);
            if(cur_read_count < 0)
            {
                return cur_read_count;
            }
            if(m_read_length + cur_read_count >= m_header.size())
            {
                m_header.resize(m_header.size() + ReadBufLength);
            }

            memcpy(const_cast<char*>(m_header.data() + m_read_length), m_buf, cur_read_count);
            m_read_length += cur_read_count;

            string::npos == m_header.find(HTTP_HEADER_EOF);

        } while(string::npos != eof_pos);

        eof_pos += HTTP_HEADER_EOF.size();
        size_t recv_str_length = strlen(m_header.c_str());
        
        if(eof_pos > recv_str_length)
        {
            //buf container \0 error
        }
        if(eof_pos < recv_str_length)
        {
            string tmp_header = m_header.substr(0, eof_pos);
            memcpy(const_cast<char*>(m_header.data() + eof_pos), m_boday, m_read_length - eof_pos);
            m_header.resize(eof_pos);
        }
        m_is_has_some_body = m_read_length > m_header.size();
        return 0;
    }
    int get_boday(ostream& os)
    {
        if(m_is_has_some_body)
        {
            m_is_has_some_body = false;
            size_t remain_count = m_read_length - m_header.size();
            os.write(m_boday, remain_count);
            return remain_count;
        }
        else
        {
            memset(m_buf, 0, ReadBufLength);
            int cur_length = m_socket_stream->write(m_buf, ReadBufLength, m_timeout);
            if(cur_length <= 0)
            {
                return StreamError::read_error;
            }
            os.write(m_buf, cur_length);
            m_read_length += cur_length;

        }
        if(m_read_length == m_content_length)
        {
            return EOF;
        }
        return 0;
    }
    const string& header()
    {
        return m_header;
    }
    const string& boday()
    {
        return m_boday;
    }
private:
    void get_content_length()
    {
        size_t pos_s = string::npos;
        pos_s = m_header.find(HTTP_KET_CONTENT_LENGTH);
        if(pos_s != string::npos)
        {
            pos_s += HTTP_KET_CONTENT_LENGTH.size();
            size_t pos_e = m_header.find(HTTP_HEADER_BREAK, pos_s);
            string str_length = m_header.substr(pos_s, pos_e - pos_s);
            size_t pos = str_length.find(':');
            if(pos != string::npos)
            {
                str_length = str_length.substr(pos);
                m_content_length = stoul(str_length);
            }
        }
    }
public:

protected:
    string m_header;
    char * m_boday;
private:
    char * m_buf;

    shared_ptr<TcpSocketStream> m_socket_stream; 
    int m_timeout;
    size_t m_read_length = 0;
    bool m_is_has_some_body = false;

    size_t m_content_length = -1;
};

}

#endif
