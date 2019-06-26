#ifndef _QYUTIL_PINGER_H_
#define _QYUTIL_PINGER_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <chrono>
#include <vector>

#include "icmp_header.hpp"
#include "ipv4_header.hpp"

#include "qyutil.h"
#include "qylog.h"


using namespace std;

typedef int RawSocket;

static unsigned short __getpid()
{
    return static_cast<unsigned short>(::getpid());
}

template<int IP_TYPE=IPPROTO_ICMP, int IS_ANDROID=0>
class pinger
{
public:
    pinger(string host, int times, int package_size, int interval/*S*/, int timeout/*S*/) : \
    _host(host), _package_size(package_size), _interval(interval), _timeout(timeout), _sequence_number(0)
    {
        _times = times == 0 ? 4 : times;
        _send_buf = new char[send_len];
        _recv_buf = new char[recv_len];
        start();
    }
    ~pinger()
    {
        if(_send_buf) delete [] _send_buf;
        if(_recv_buf) delete [] _recv_buf;
        close(_socket);
    }
    int start()
    {
        int start_ret = 0;
        do
        {
            _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
            if(0 == _socket)
            {
                LOGE("%s", "create socket error!\n");
                cout << "create socket error!" << endl;
                break;
            }
            timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, &recv_len, sizeof(recv_len));
            setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            for (int i = 0; i < _times; ++i)
            {
                send_data();
            }
        }
        while (false);
        return start_ret;
    }
    int send_data()
    {
        int send_ret = 0;
        int read_ret = 0;
        do{
            std::string body("test link\n");
            
            // Create an ICMP header for an echo request.
            icmp_header echo_request;
            echo_request.type(icmp_header::echo_request);
            echo_request.code(0);
            echo_request.identifier(__getpid());
            echo_request.sequence_number(++_sequence_number);
            compute_checksum(echo_request, body.begin(), body.end());
            memset(_send_buf, 0, send_len);
            int request_len = echo_request.write(_send_buf);
            strcpy(request_len + _send_buf, body.c_str());
            sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
//            long laddr = inet_addr(_host.c_str());
//            addr.sin_family = AF_INET;
//            addr.sin_addr.s_addr = laddr;
//            if(laddr == INADDR_NONE)
//            {
//                break;
//            }
            if(0 >= (send_ret = inet_pton(AF_INET, _host.c_str(), &addr.sin_addr)))
            {
                LOGE("%s", "invalid address!\n");
                break;
            }
            _send_time = chrono::steady_clock::now();
            send_ret = sendto(_socket, _send_buf, request_len + body.length(), 0, (sockaddr*)&addr, sizeof(addr));
            if(send_ret <= 0)
            {
                LOGE("send fail %d host:%s", send_ret, _host.c_str());
                break;
            }
            read_ret = read_data();
            if(read_ret < 0)
            {
                send_ret = read_ret;
                break;
            }
        }
        while (false);
        return send_ret;
    }
    int read_data()
    {
        int read_ret = 0;
        do
        {
            ipv4_header ipv4_hdr;
            icmp_header icmp_hdr;
            
            sockaddr addr;
            socklen_t addr_len = 0;
            memset(_recv_buf, 0, recv_len);
            read_ret = recvfrom(_socket, _recv_buf, recv_len, 0, (struct sockaddr*)&addr, &addr_len);
            if(read_ret < 0)
            {
                LOGI("ping host:%s timeout", _host.c_str());
                break;
            }
            stringbuf buf;
            istream stream(&buf);
            buf.sputn(_recv_buf, read_ret);
            stream >> ipv4_hdr >> icmp_hdr;
            
            if (stream && icmp_hdr.type() == icmp_header::echo_reply
                && icmp_hdr.identifier() == __getpid()
                && icmp_hdr.sequence_number() == _sequence_number)
            {
                
                // Print out some information about the reply packet.
                chrono::steady_clock::time_point now = chrono::steady_clock::now();
                //chrono::steady_clock::duration elapsed = now - time_sent_;
                
                stringstream ss;
                ss << ipv4_hdr.header_length()
                << " bytes from " << ipv4_hdr.source_address()
                << ": icmp_seq=" << icmp_hdr.sequence_number()
                << ", ttl=" << ipv4_hdr.time_to_live()
                << ", time=" << ((chrono::duration_cast<chrono::milliseconds>)(chrono::steady_clock::now() - _send_time)).count() << "ms"
                << std::endl;
                
                std::cout << ss.str();

                LOGE("%s", _host.c_str());
                LOGE("%s", ss.str().c_str());
             
                
            }
        }
        while(false);
        return read_ret;
    }
    RawSocket _socket;
    string _host;
    int _times;
    int _package_size;
    int _interval;
    int _timeout;
    int _sequence_number;
    
    chrono::time_point<chrono::steady_clock> _send_time;
    
    char * _send_buf;
    char * _recv_buf;
    const size_t send_len = 1024;
    const size_t recv_len = 65536;
};

template<int IP_TYPE>
class pinger<IP_TYPE, 1>
{
public:
    pinger(string host, int times, int package_size, int interval/*S*/, int timeout/*S*/)
    {
        stringstream ss;
        //int index = snprintf(cmd, 256, "ping -c %d -i %d -w %d", _querycount, interval, timeout);
        ss << "ping " << " -c " << times << " -i " <<  interval << " -w " << timeout << " " << host << "\n"; 

        const char * cmd = ss.str().c_str();
        LOGE("----------------cmd:%s", cmd);
        FILE* pp = popen(cmd, "r");
        if(!pp)
        {
            LOGE("open %s fail", cmd);
        }
        char line[1024] = { 0 };
        while (fgets(line, sizeof(line), pp) != NULL)
        {
            _result.push_back(string(line));
            LOGE("ping result:%s", line);
            memset(line, 0, sizeof(line));            
        }
        pclose(pp);
        
    }
    vector<string> _result;
};

#endif


