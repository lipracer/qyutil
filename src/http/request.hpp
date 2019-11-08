#ifndef _QYUTIL_REQUEST_
#define _QYUTIL_REQUEST_

#include "net_common/net_common.h"
#include <string>
#include <map>
#include <sstream>
#include <memory>

#include "qylog.h"
#include "qyutil.h"
#include "../dnsquery/dnsquery.h"

#define HTTP_HEADER_EOF ("\r\n")

//#define _qyinfo

namespace qyutil
{
using namespace std;
using namespace NetCommon;

enum StreamError : int
{
    ok = 0,
    sock_invalid,
    connect_error,
    send_error,
    read_error,
};

const size_t LTcpBuf = 1024 + 1;

class SocketStream
{
public:
    virtual StreamError open(string ip, short port, int timeout) = 0;
    virtual StreamError close() = 0;
    virtual StreamError read(const char *buf, size_t& length, int timeout) = 0;
    virtual StreamError write(const char *buf, size_t& length, int timeout) = 0;
};

class TcpSocketStream : public SocketStream
{
public:
    StreamError open(string ip, short port, int timeout) override;
    StreamError close() override;
    StreamError read(const char *buf, size_t& length, int timeout) override;
    StreamError write(const char *buf, size_t& length, int timeout) override;
private:
    RawSocket hsocket;
};

class Request
{
    static const short DEFAULT_PORT = 80;
public:
    static string& version()
    {
        static string version = "1.0";
        return version;
    }
    Request() : port(Request::DEFAULT_PORT)
    {
        stream = make_shared<TcpSocketStream>();
        //buf = shared_ptr<char>(new char[LTcpBuf]);
        m_buf = new char[LTcpBuf];
    }
    ~Request()
    {
        delete [] m_buf;
        _qyinfo("destruct request");
    }
    void get(string url, map<string, string> params, int timeout)
    {
        this->timeout = timeout;
        parse_url(url);
        build_req_header(url, 0, params);
        _qyinfo(header.str());
        if(ips.size())
        {
            auto first = ips.begin();
            while (first != ips.end() && stream->open(*first, port, timeout))
            {
                first++;
            }
            if(first != ips.end())
            {
                send_data();
                parse_req_header();
            }
        }
    }
    void post(string url, map<string, string> params, int timeout)
    {
        this->timeout = timeout;
        build_req_header(url, 1, params);
    }
private:
    int get_ip_by_domain(string& domain)
    {
        socket_ipinfo_t info;
        info.size = 0;
        int result = socket_gethostbyname(domain.c_str(),&info, timeout, "10.16.169.127");
        for(int i = 0; i < info.size; ++i)
        {
            ips.push_back(info.ip[i]);
        }
        return result;
    }

    // /(http|https):\/\/([\w.]+\/?)\S*/　
    void parse_url(string &url)
    {
        string prefix = url.substr(0, 4);
        if(prefix != "http")
        {
            string msg = "bad url:";
            msg += url;
            throw QYUtilException(msg);
        }
        if(url.substr(0, 5) == "https")
        {
            string msg = "not support url:";
            msg += url;
            throw QYUtilException(msg);
        }
        string domain = url.substr(7, url.length() - 7);
        size_t position = domain.find('/');
        if(std::string::npos != position)
        {
            domain = domain.substr(0, position);
        }
        position = domain.find(':'); 
        if(std::string::npos != position)
        {
            if(position + 1 < domain.length())
            {
                string str_port = domain.substr(position + 1);
                short nPort = atoi(str_port.c_str());
                if(nPort > 0) port = nPort;
            }
        }
//        ips.push_back("127.0.0.1");
        if(get_ip_by_domain(domain))
        {
            string msg = "unknown domain:";
            msg += domain;
            throw QYUtilException(msg);
        }
    }
    void build_req_header(string& url, int method, map<string, string>& params)
    {
        const char * method_name[] = { "GET", "POST", "PUT", "DELETE" };
        string path = url.substr(7, url.length() - 7);
        size_t back_slash = path.find('/');
        if(std::string::npos != back_slash)
        {
            path = path.substr(back_slash, path.length() - back_slash);
            path += "?";
            for (auto& it : params)
            {
                path += it.first + "=" + it.second;
                path += "&";
            }
            path = path.substr(0, path.length() - 1);
        }
        else
        {
            path = "/";
        }
        header << method_name[method];
        header << " /";
        header << " HTTP/" << Request::version();
        header << HTTP_HEADER_EOF;
        header << HTTP_HEADER_EOF;
    }
    int send_data()
    {
        size_t length = header.str().length();
        if(StreamError::ok != stream->write(header.str().c_str(), length, timeout))
        {
            throw QYUtilException("send error");
        }
        
        memset(m_buf, 0, LTcpBuf);
        length = LTcpBuf - 1;
        
        do
        {
            if(StreamError::ok != stream->read(m_buf, length, timeout))
            {
                throw QYUtilException("recv error");
            }
            response.append(m_buf, length);
            memset(m_buf, 0, LTcpBuf);
        }while(length == LTcpBuf);
        cout << response << "---------------------------------------" << endl;
        _qyinfo(response);
        return 0;
    }
    void parse_req_header()
    {
        size_t eof_pos = response.find("\r\n\r\n");
        if(eof_pos == string::npos)
        {
            throw QYUtilException("bad response");
        }
        string header = response.substr(0, eof_pos);
        _qyinfo(header, header.length());
    }
public:
    
private:
    shared_ptr<SocketStream> stream;
    stringstream header;
    int code;
    int timeout;
    list<string> ips;
    short port;
    char *m_buf;
    string response;
};

};

#endif
