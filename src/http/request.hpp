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

const size_t LTcpBuf = 1024;

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
public:
    static string& version()
    {
        static string version = "1.0";
        return version;
    }
    Request()
    {
        stream = make_shared<TcpSocketStream>();
        //buf = shared_ptr<char>(new char[LTcpBuf]);
        buf = make_shared<char>(LTcpBuf);
    }
    ~Request(){ _qyinfo("destruct request"); }
    void get(string url, map<string, string> params, int timeout)
    {
        this->timeout = timeout;
        parse_url(url);
        build_req_header(url, 0, params);
        _qyinfo(header.str());
        if(ips.size())
        {
            auto first = ips.begin();
            while (first != ips.end() && stream->open(*first, 8000, timeout))
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
        int result = socket_gethostbyname(domain.c_str(),&info, timeout, "8.8.8.8");
        for(int i = 0; i < info.size; ++i)
        {
            ips.push_back(info.ip[i]);
        }
        return result;
    }
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
        size_t first_backslash = domain.find('/');
        if(std::string::npos != first_backslash)
        {
            domain = domain.substr(0, first_backslash);
        }
        ips.push_back("10.4.155.105");
        if(get_ip_by_domain(domain))
        {
            string msg = "unknown domain:";
            msg += domain;
            //throw QYUtilException(msg);
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
        
        memset(buf.get(), 0, LTcpBuf);
        length = LTcpBuf;
        
        do
        {
            if(StreamError::ok != stream->read(buf.get(), length, timeout))
            {
                throw QYUtilException("recv error");
            }
            response.append(buf.get(), length);
        }while(length == LTcpBuf);
        response += '\0';
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
    shared_ptr<char> buf;
    string response;
};

};

#endif
