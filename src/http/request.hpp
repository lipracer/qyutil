#ifndef _QYUTIL_REQUEST_
#define _QYUTIL_REQUEST_

#include "response.hpp"

#include "net_common/net_common.h"
#include <string>
#include <map>
#include <sstream>
#include <memory>

#include "qylog.h"
#include "qyutil.h"
#include "../dnsquery/dnsquery.h"
#include "SocketStream.hpp"

//#define _qyinfo

namespace qyutil
{
using namespace std;
using namespace NetCommon;

const size_t LTcpBuf = 1024 + 1;

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
        response response(stream, timeout);
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
                response.get_header();
                response.parse_header();
                
                stringstream ss;
                while(EOF !=  response.get_boday(ss))
                {
                    //cout << ss.tellp() << endl;
                }
                cout << ss.str() << endl;
 
            }
        }
        //return response(str_response);
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
#if 0
        int result = socket_gethostbyname(domain.c_str(),&info, timeout, "10.16.169.127");
#else
        int result = socket_gethostbyname(domain.c_str(),&info, timeout, "8.8.8.8");
#endif
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
        header << HTTP_HEADER_BREAK;
        header << "method:" << method_name[method] << HTTP_HEADER_BREAK;
        header << "Connection: keep-alive" << HTTP_HEADER_BREAK;
        header << "User-Agent: Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Mobile Safari/537.36" << HTTP_HEADER_BREAK;
        header << HTTP_HEADER_EOF;
    }
    int send_data()
    {
        size_t length = header.str().length();
        if(0 >= stream->write(header.str().c_str(), length, timeout))
        {
            throw QYUtilException("send error");
        }
        
        memset(m_buf, 0, LTcpBuf);
        length = LTcpBuf - 1;
        return 0;
    }
public:
    
private:
    shared_ptr<TcpSocketStream> stream;
    stringstream header;
    int code;
    int timeout;
    list<string> ips;
    short port;
    char *m_buf;
    string str_response;
};
};
#endif
