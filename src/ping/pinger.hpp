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
#include <functional>
#include <asio.hpp>

#include "net_common/icmp_header.hpp"
#include "net_common/ipv4_header.hpp"
#include "net_common/net_common.h"

#include "qyutil.h"
#include "qylog.h"

using namespace NetCommon;
using namespace std;

template<int IP_TYPE=IPPROTO_ICMP, int IS_ANDROID=0>
class pinger
{
public:
    struct PingState
    {
        u64 cost_time;
        bool bsend;
        bool brecv;
    };
    pinger(string host, int times, int package_size, int interval/*S*/, int timeout/*S*/, shared_ptr<result_output>& output) : \
    _host(host), _package_size(package_size), _interval(interval), _timeout(timeout), _sequence_number(0), _output(output)
    {
        _times = times == 0 ? 4 : times;
        _send_buf = new char[send_len];
        _recv_buf = new char[recv_len];
        if(_times) start();
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
            timeout.tv_sec = _interval;
            timeout.tv_usec = 0;
            setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, &recv_len, sizeof(recv_len));
            setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            _body = "test link\n";
            CommonOutPut("PING %s (%s): %d data bytes\n", _host.c_str(), _host.c_str(), _body.length());
            for (int i = 0; i < _times; ++i)
            {
                PingState state = { 0 };
                auto sp = std::chrono::steady_clock::now();
                int ret = send_data();
                if(0 < ret)
                {
                    ret = read_data();
                    if(ret > 0)
                    {
                        auto ep = std::chrono::steady_clock::now();
                        u64 duration = (std::chrono::duration_cast<std::chrono::milliseconds>(ep - sp)).count();
                        stringstream ss;
                        ss << _recv_ipv4_hdr.header_length()
                        << " bytes from " << _recv_ipv4_hdr.source_address()
                        << ": icmp_seq=" << _recv_icmp_hdr.sequence_number()
                        << ", ttl=" << _recv_ipv4_hdr.time_to_live()
                        << ", time=" << duration << "ms";
                        CommonOutPut("%s\n", ss.str().c_str());
                        state.bsend = true;
                        state.brecv = true;
                        state.cost_time = duration;
                    }
                }
                _result.push_back(state);
            }
        }
        while (false);
        statistics();
        return start_ret;
    }
    int send_data()
    {
        int send_ret = 0;
        do{            
            // Create an ICMP header for an echo request.
            icmp_header echo_request;
            echo_request.type(icmp_header::echo_request);
            echo_request.code(0);
            echo_request.identifier(__getpid());
            echo_request.sequence_number(++_sequence_number);
            compute_checksum(echo_request, _body.begin(), _body.end());
            memset(_send_buf, 0, send_len);
            int request_len = echo_request.write(_send_buf);
            strcpy(request_len + _send_buf, _body.c_str());
            sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            if(0 >= (send_ret = inet_pton(AF_INET, _host.c_str(), &addr.sin_addr)))
            {
                LOGE("%s", "invalid address!\n");
                break;
            }
            send_ret = sendto(_socket, _send_buf, request_len + _body.length(), 0, (sockaddr*)&addr, sizeof(addr));
            if(send_ret <= 0)
            {
                LOGE("send fail %d host:%s", send_ret, _host.c_str());
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
            sockaddr addr;
            socklen_t addr_len = 0;
            memset(_recv_buf, 0, recv_len);
            read_ret = recvfrom(_socket, _recv_buf, recv_len, 0, (struct sockaddr*)&addr, &addr_len);
            if(read_ret < 0)
            {
                //ScreenOutput("ping host:%s timeout", _host.c_str());
                break;
            }
            stringbuf buf;
            istream stream(&buf);
            buf.sputn(_recv_buf, read_ret);
            stream >> _recv_ipv4_hdr >> _recv_icmp_hdr;
            
            if (stream && _recv_icmp_hdr.type() == icmp_header::echo_reply
                && _recv_icmp_hdr.identifier() == __getpid()
                && _recv_icmp_hdr.sequence_number() == _sequence_number)
            {
                read_ret = _recv_ipv4_hdr.total_length();
            }
        }
        while(false);
        return read_ret;
    }
    void statistics()
    {
        CommonOutPut("\n--- %s ping statistics ---\n", _host.c_str());
        int sendc = 0, recvc = 0;
        u64 max = 0, min = 10000, avg = 0;
        for(auto& it : _result)
        {
            if(it.bsend) ++sendc;
            if(it.brecv) ++recvc;
            if(it.bsend && it.brecv)
            {
                if(max < it.cost_time) max = it.cost_time;
                if(min > it.cost_time) min = it.cost_time;
                avg += it.cost_time;
            }
        }
        float loss = (2.0 * _times - sendc - recvc) / (2.0 * _times) * 100;
        CommonOutPut("%d packets transmitted, %d packets received, %0.1f%s", sendc, recvc, loss, "%% packet loss\n");
        CommonOutPut("round-trip min/avg/max/stddev = %lld/%0.2f/%lld/6.420 ms", min, (double)avg / _times, max);
        
    }
    RawSocket _socket;
    string _host;
    int _times;
    int _package_size;
    int _interval;
    int _timeout;
    int _sequence_number;
    string _body;
    ipv4_header _recv_ipv4_hdr;
    icmp_header _recv_icmp_hdr;
    
    char * _send_buf;
    char * _recv_buf;
    const size_t send_len = 1024;
    const size_t recv_len = 65536;
    shared_ptr<result_output> _output;
    char *_output_buf;

    list<PingState> _result;
};

template<int IP_TYPE>
class pinger<IP_TYPE, 1>
{
public:
    pinger(string host, int times, int package_size, int interval/*S*/, int timeout/*S*/, shared_ptr<result_output>& output) : _output(output)
    {
        stringstream ss;
        //int index = snprintf(cmd, 256, "ping -c %d -i %d -w %d", _querycount, interval, timeout);
        if(0 == timeout)
        {
            timeout = interval * times;
        }
        ss << "ping " << " -c " << times << " -i " <<  interval << " -w " << timeout << " " << host << "\n"; 

        const char * cmd = ss.str().c_str();
        LOGI("cmd:%s", cmd);
        FILE* pp = popen(cmd, "r");
        if(!pp)
        {
            LOGE("open %s fail", cmd);
        }
        char line[1024] = { 0 };
        string str_result;
        while (fgets(line, sizeof(line), pp) != NULL)
        {
            _result.push_back(string(line));
            str_result += line;
            memset(line, 0, sizeof(line));            
        }
        pclose(pp);
        CommonOutPut("ping result:%s", str_result.c_str());
    }
public:
    vector<string> _result;
    shared_ptr<result_output> _output;
};

extern "C"
{    
/*
 @param host : host ip (v4)
 */
//int qy_sync_ping(const char* host, int times, int package_size, int interval/*S*/, int timeout/*S*/);
//
//int qy_async_ping(const char* host, int times, int package_size, int interval/*S*/, int timeout/*S*/);

}

#endif


