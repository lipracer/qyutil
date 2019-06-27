#ifndef _QYUTIL_TRACEROUTER_
#define _QYUTIL_TRACEROUTER_

/*
UDP or ICMP
 */

#include <string>
#include <asio/ip/address_v4.hpp>
#include "qylog.h"
#include "net_common/net_common.h"

using namespace std;
using namespace NetCommon;

const static int BUF_LEN = 65536;

template<NetCommon::TraceRouterType ProType>
class TraceRouter
{
public:
    TraceRouter(TraceRouter& other)=delete;
    TraceRouter(TraceRouter&& other)=delete;
    TraceRouter& operator=(TraceRouter& other)=delete;
    TraceRouter& operator=(TraceRouter&& other)=delete;
    TraceRouter(string host) : _host(host)
    {
        _send_buf = new char[BUF_LEN];
        _recv_buf = new char[BUF_LEN];
        start();
    }


    virtual ~TraceRouter()
    {
        if(_send_buf) delete [] _send_buf;
        if(_recv_buf) delete [] _recv_buf;
    }

    int start()
    {
        int ret = 0;
        do
        {
            _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if(0 == _socket)
            {
                LOGE("%s", "create socket error!\n");
                cout << "create socket error!" << endl;
                break;
            }
            int opt = 1;
            setsockopt(_socket, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt));
            setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, &BUF_LEN, sizeof(BUF_LEN));
            timeval timeout;
            timeout.tv_sec = TraceRouterTimeOut;
            timeout.tv_usec = 0;
            setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            
            explore_router(1);
        }
        while(false);
        return ret;
    }

    int explore_router(int index)
    {
        int ret = 0;
        do
        {
            sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));

            if(0 >= (ret = inet_pton(AF_INET, _host.c_str(), &addr.sin_addr)))
            {
                LOGE("%s", "invalid address!\n");
                break;
            }
            
            memset(_send_buf, 0, BUF_LEN);
            
            u8 un_head[] = { 0x74, 0xea, 0xcb, 0xd0, 0x30, 0x90, 0xf0, 0x18, 0x98, 0x59, 0x2a, 0xcd, 0x08, 0x00 };
            static_assert(sizeof(un_head) == 14, "");
            memcpy(_send_buf, un_head, sizeof(un_head));
            print_buf();
            
            PROP_IP_HEADER *ip_head = operator new(_send_buf + sizeof(un_head));
            ip_head->ttl = index & 0XFF;
            print_buf();

            UDP_HAEDER udp_head;;
            udp_head.sour_port = htons(DefaultPort_S);
            udp_head.dest_port = htons(DefaultPort_D);
            udp_head.udp_len = htons(0x20);
            udp_head.check_sum = htons(0xf9ba);
            memcpy(_send_buf + sizeof(un_head) + sizeof(ip_head), &udp_head, sizeof(udp_head));
            print_buf();

            int data_len = sizeof(un_head) + sizeof(ip_head) + sizeof(udp_head) + 24;
            ((PROP_IP_HEADER*)(_send_buf + sizeof(un_head)))->pack_len = htons(52);
            ((PROP_IP_HEADER*)(_send_buf + sizeof(un_head)))->check_sum = htons(0x3661);
            ((PROP_IP_HEADER*)(_send_buf + sizeof(un_head)))->sour_ip = htonl(asio::ip::make_address_v4("10.4.156.103").to_ulong());
            ((PROP_IP_HEADER*)(_send_buf + sizeof(un_head)))->dest_ip = htonl(asio::ip::make_address_v4("115.239.210.27").to_ulong());
            
            print_buf();

            ret = sendto(_socket, _send_buf, data_len, 0, (sockaddr*)&addr, sizeof(addr));
            if(ret <= 0)
            {
                LOGE("send fail %d host:%s", ret, _host.c_str());
                break;
            }
        }
        while (false);
        return ret;
    }

    int recv_data()
    {

    }
    
    void print_buf()
    {
        int i = 0;
        while (i < 52 + 14)
        {
            if(i % 16 == 0)
            {
                printf("\n");
            }
            printf("%02X ", 0x000000ff & _send_buf[i]);
            ++i;
        }
        cout << endl;
    }

protected:
    string _host;
    char * _send_buf;
    char * _recv_buf;
    RawSocket _socket;
    
static const int TraceRouterTimeOut = 1;
static const unsigned short DefaultPort_S = 38625;
static const unsigned short DefaultPort_D = 33435;
};

#endif
