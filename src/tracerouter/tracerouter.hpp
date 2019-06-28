#ifndef _QYUTIL_TRACEROUTER_
#define _QYUTIL_TRACEROUTER_

/*
UDP or ICMP
 */

#include <string>
#include <asio/ip/address_v4.hpp>

#include "qylog.h"
#include "net_common/ipv4_header.hpp"
#include "net_common/icmp_header.hpp"
#include "net_common/net_common.h"

using namespace std;
using namespace NetCommon;

extern int errno;

const static size_t BUF_LEN = 65536;
uint16_t ip_chksum(uint16_t initcksum, uint8_t *ptr, int len)
{
    unsigned int cksum;
    int idx;
    int odd;
    cksum = (unsigned int) initcksum;
    odd = len & 1;
    len -= odd;
    for (idx = 0; idx < len; idx += 2)
        cksum += ((unsigned long) ptr[idx] << 8) + ((unsigned long) ptr[idx+1]);
    
    if (odd)
        cksum += ((unsigned long) ptr[idx] << 8);
    
    while (cksum >> 16)
        cksum = (cksum & 0xFFFF) + (cksum >> 16);
    
    return cksum;
}

template<TraceRouterType ProType=TraceRouterType::UDP>
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
        _data_len = 24;
        _sequence_number = 0;
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
            _socket_w = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            _socket_r = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
            if(0 == _socket_w)
            {
                LOGE("%s", "create socket error!\n");
                cout << "create socket error!" << endl;
                break;
            }
            //原始套接字必须root权限
//            int opt = 1;
//            if(0 > (ret = setsockopt(_socket, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt))))
//            {
//                LOGE("set raw socket error!!!");
//                break;
//            }
            timeval timeout;
            timeout.tv_sec = TraceRouterTimeOut;
            timeout.tv_usec = 0;
            if(0 > (ret = setsockopt(_socket_r, SOL_SOCKET, SO_RCVBUF, &BUF_LEN, sizeof(BUF_LEN))))
            {
                LOGE("set socket recv timeout error!!!");
                break;
            }
            if(0 > (ret = setsockopt(_socket_r, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))))
            {
                LOGE("set socket recv timeout error!!!");
                break;
            }
            //prepare_ip_udp_packet();
            for(int i = 1; i < 50; i++)
            {
//                _ip_head->ttl = (0XFF & i);
//                _ip_head->check_sum = htons(ip_chksum(0, (uint8_t*)_send_buf, sizeof(PROP_IP_HEADER)));
                int ttl = 0XFF & i;
                if(0 > (ret = setsockopt(_socket_w, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl))))
                {
                    LOGE("set socket IP_TTL error:%d!!!", errno);
                    goto EXIT;
                }
                string t_ip;
                explore_router(i, t_ip);
                if(t_ip == _host)
                {
                    ret = 0;
                    break;
                }
            }
        }
        while(false);
    EXIT:
        return ret;
    }
    
    int prepare_ip_udp_packet()
    {
        memset(_send_buf, 0, BUF_LEN);
        
        _ip_head = new(_send_buf) PROP_IP_HEADER();
        _ip_head->pack_id = htons(0xa562);
        
        _udp_head = new(_send_buf + sizeof(PROP_IP_HEADER)) UDP_HAEDER();
        _udp_head->sour_port = htons(DefaultPort_S);
        _udp_head->dest_port = htons(DefaultPort_D);
        _udp_head->udp_len = htons(sizeof(UDP_HAEDER));
        _udp_head->check_sum = htons(0xf9ba);
        
        _ip_head->pack_len = htons(sizeof(PROP_IP_HEADER) + sizeof(UDP_HAEDER) + _data_len);
        _ip_head->sour_ip = htonl(asio::ip::make_address_v4("10.4.156.103").to_ulong());
        _ip_head->dest_ip = htonl(asio::ip::make_address_v4(_host.c_str()).to_ulong());
        return 0;
    }

    int explore_router(int index, string& t_ip)
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
            addr.sin_port = htons(DefaultPort_D);
            
            string body = "xxx";
            
            ret = sendto(_socket_w, _send_buf, body.length(), 0, (sockaddr*)&addr, sizeof(addr));
            if(ret <= 0)
            {
                LOGD("send fail %d host:%s errno:%d", ret, _host.c_str(), errno);
                break;
            }
            memset(_recv_buf, 0, BUF_LEN);
            socklen_t addr_len = 0;
            ret = recvfrom(_socket_r, _recv_buf, BUF_LEN, 0, (struct sockaddr*)&addr, &addr_len);
            if(ret <= 0)
            {
                //LOGD("******* recv error!");
                cout << index << "      **********" << endl;
                break;
            }
            stringbuf buf;
            istream stream(&buf);
            buf.sputn(_recv_buf, ret);
            ipv4_header ipv4_hdr;
            icmp_header icmp_hdr;
            stream >> ipv4_hdr >> icmp_hdr;
            t_ip = ipv4_hdr.source_address().to_string();
            cout << index << "      router addr:" << ipv4_hdr.source_address().to_string() << endl;
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
        while (i < sizeof(PROP_IP_HEADER) + sizeof(UDP_HAEDER) + _data_len)
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
    RawSocket _socket_w;
    RawSocket _socket_r;
    PROP_IP_HEADER * _ip_head;
    UDP_HAEDER * _udp_head;
    int _data_len;
    int _sequence_number;
    
static const int TraceRouterTimeOut = 5;
static const unsigned short DefaultPort_S = 38625;
static const unsigned short DefaultPort_D = 33435;
};

#endif
