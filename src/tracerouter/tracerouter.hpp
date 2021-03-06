#ifndef _QYUTIL_TRACEROUTER_
#define _QYUTIL_TRACEROUTER_

/*
UDP or ICMP
func1 : 原生套节子，修改ip协议头部ttl字段
func2 : 使用setsockopt修改ip字段
func3 : 基于TCP，修改SYN TTL，设置socket混合模式
 */

#include <vector>
#include <list>
#include <string>
//#include <asio/ip/address_v4.hpp>

#include "qylog.h"
#include "net_common/ipv4_header.hpp"
#include "net_common/icmp_header.hpp"
#include "net_common/net_common.h"

using namespace std;
using namespace NetCommon;

extern int errno;

const static size_t BUF_LEN = 65536;
const static int    EveryTTLExpCount = 3;
const static int    MaxTraceTTL      = 20;
//uint16_t ip_chksum(uint16_t initcksum, uint8_t *ptr, int len)
//{
//    unsigned int cksum;
//    int idx;
//    int odd;
//    cksum = (unsigned int) initcksum;
//    odd = len & 1;
//    len -= odd;
//    for (idx = 0; idx < len; idx += 2)
//        cksum += ((unsigned long) ptr[idx] << 8) + ((unsigned long) ptr[idx+1]);
//    
//    if (odd)
//        cksum += ((unsigned long) ptr[idx] << 8);
//    
//    while (cksum >> 16)
//        cksum = (cksum & 0xFFFF) + (cksum >> 16);
//    
//    return cksum;
//}

#ifdef __ANDROID__ 
#define SO_NOSIGPIPE (0)
#endif

template<TraceRouterType ProType=TraceRouterType::UDP, int IS_ANDROID=0>
class TraceRouter
{
public:
    TraceRouter(TraceRouter& other)=delete;
    TraceRouter(TraceRouter&& other)=delete;
    TraceRouter& operator=(TraceRouter& other)=delete;
    TraceRouter& operator=(TraceRouter&& other)=delete;
    TraceRouter(string host, shared_ptr<result_output>& output) : _host(host), _output(output)
    {
        LOGI("TraceRouter contruct");
        _send_buf = new char[BUF_LEN];
        _recv_buf = new char[BUF_LEN];
        _data_len = 24;
        _sequence_number = 0;
        start();
    }

    virtual ~TraceRouter()
    {
        LOGI("TraceRouter ~TraceRouter");
        if(_send_buf) 
        {
            delete [] _send_buf;
            _send_buf = nullptr;
        }
        if(_recv_buf) 
        {
            delete [] _recv_buf;
            _recv_buf = nullptr;
        }
        socket_close(_socket_r);
        socket_close(_socket_w);
    }   

    int start()
    {
        LOGI("TraceRouter start");
        int ret = 0;
        do
        {
            _socket_w = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            _socket_r = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
            if(0 == _socket_w || 0 == _socket_r)
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
//            if(0 > (ret = setsockopt(_socket_r, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))))
//            {
//                LOGE("set socket recv timeout error!!!");
//                break;
//            }
            //prepare_ip_udp_packet();
            //only ios
            const int value = 1;
            if(0 > (ret = setsockopt(_socket_w, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(int))))
            {
                break;
            }
            for(int i = 1; i < MaxTraceTTL; i++)
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
                //到达主机，退出循环
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
#if 0   
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
#endif
    int explore_router(int index, string& t_ip)
    {
        LOGI("TraceRouter explore_router");
        int ret = 0;
        do
        {
            CommonOutPut("%2d ", index);///////////////////////
            sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));

            if(0 >= (ret = inet_pton(AF_INET, _host.c_str(), &addr.sin_addr)))
            {
                LOGE("invalid address!\n");
                break;
            }
            addr.sin_port = htons(DefaultPort_D);
            char data[50] = { 0 };
            
            int times = EveryTTLExpCount;
            while (times--)
            {
                auto start_pt =  std::chrono::system_clock::now();
                LOGI("TraceRouter sendto");
                ret = sendto(_socket_w, _send_buf, sizeof(data), 0, (sockaddr*)&addr, sizeof(addr));
                if(ret <= 0)
                {
                    LOGE("send fail ret:%d host:%s errno:%d", ret, _host.c_str(), errno);
                    CommonOutPut("traceroute: %s %d chars, ret=%d", _host.c_str(), (int)sizeof(data), ret);////////////////
                    break;
                }
                memset(_recv_buf, 0, BUF_LEN);
                socklen_t addr_len = 0;
                LOGI("TraceRouter recvfrom");
                ret = RecvWithinTime(_socket_r, _recv_buf, BUF_LEN, (struct sockaddr*)&addr, &addr_len, TraceRouterTimeOut, 0);
                auto end_pt =  std::chrono::system_clock::now();
                if(ret <= 0)
                {
                    LOGI("TraceRouter recvfrom ret: errno:%d", ret, errno);
                    CommonOutPut("%s", " *");
                    continue;
                }
                LOGI("TraceRouter recvfrom ret:%d", ret);
                //TODO 检查ICMP包是否是路由响应的包、否则重新接收?
                stringbuf buf;
                istream stream(&buf);
                buf.sputn(_recv_buf, ret);
                ipv4_header ipv4_hdr;
                icmp_header icmp_hdr;
                stream >> ipv4_hdr >> icmp_hdr;
                stringstream ss;
                ss << ipv4_hdr.source_address();
                string cur_ip = ss.str();
                if(t_ip == cur_ip)
                {
                    
                }
                else
                {
                    //printf %s how to align ????
                    t_ip = cur_ip;
                    if(EveryTTLExpCount - 1 != times)
                    {
                        CommonOutPut("%s", "\n   ");
                        
                    }
                    CommonOutPut(" %s (%s)", t_ip.c_str(), t_ip.c_str());
                }
                int duration = (std::chrono::duration_cast<std::chrono::microseconds>(end_pt - start_pt)).count();
                CommonOutPut("  %0.2fms", duration / (float)1000);
            }
            CommonOutPut("%s", "\n");
          
        }
        while (false);
        return ret;
    }
    
    int RecvWithinTime(int _fd, char* _buf, size_t _buf_n, struct sockaddr* _addr, socklen_t* _len, unsigned int _sec, unsigned _usec) {
        struct timeval tv;
        fd_set readfds, exceptfds;
        int n = 0;
        
        FD_ZERO(&readfds);
        FD_SET(_fd, &readfds);
        FD_ZERO(&exceptfds);
        FD_SET(_fd, &exceptfds);
        
        tv.tv_sec = _sec;
        tv.tv_usec = _usec;
        
        int ret = -1;
    label:
        ret = select(_fd + 1, &readfds, NULL, &exceptfds, &tv);
        
        if (-1 == ret) {
            if (EINTR == errno) {
                // select被信号中断 handler
                FD_ZERO(&readfds);
                FD_SET(_fd, &readfds);
                FD_ZERO(&exceptfds);
                FD_SET(_fd, &exceptfds);
                goto label;
            }
        }
        
        if (FD_ISSET(_fd, &exceptfds)) {
            // socket异常处理
            _qyerro("socket exception.");
            return -1;
        }
        
        if (FD_ISSET(_fd, &readfds)) {
            if ((n = (int)recvfrom(_fd, _buf, _buf_n, 0, _addr, _len)) >= 0) {
                return n;
            }
        }
        
        return -1;  // 超时或者select失败
    }

protected:
    string _host;
    char * _send_buf;
    char * _recv_buf;
    RawSocket _socket_w;
    RawSocket _socket_r;
    //PROP_IP_HEADER * _ip_head;
    //UDP_HAEDER * _udp_head;
    int _data_len;
    int _sequence_number;

    vector<list<string>> _router_path;
    shared_ptr<result_output> _output;
    char _output_buf[OutputBufLen];
    string __all_result;
    
static const int TraceRouterTimeOut = 5;
static const unsigned short DefaultPort_S = 38625;
static const unsigned short DefaultPort_D = 33435;
};

template<TraceRouterType TRT>
class TraceRouter<TRT, 1>
{
public:
    TraceRouter(string host, shared_ptr<result_output>& output): _host(host), _output(output)
    {
        LOGI("TraceRouter contruct");
        __fmt_ping_order = string("ping -t %d -c 1 -w 1 ") + _host;
        for(int i = 1; i < MaxTraceTTL; ++i)
        {
            string t_ip;
            explore_router(i, t_ip);
        }    
    }
    ~TraceRouter()
    {
        LOGI("TraceRouter destruct");
        LOGE("%s", __all_result.c_str());//打印不全， 可能是android_log截断了
    }

    int explore_router(int index, string& t_ip)
    {
        CommonOutPut("%2d ", index);
        int times = 3;
        while (times--)
        {
            string str_result;
            auto start_pt = std::chrono::steady_clock::now();
            open_ping_cmd(index, str_result);
            auto end_pt = std::chrono::steady_clock::now();
            string ip_;
            if(process_ping_result(str_result, ip_))
            {
                CommonOutPut("%s", " *");
            }
            else
            {                
                if(ip_ != t_ip)
                {
                    t_ip = ip_;
                    if(times != EveryTTLExpCount - 1)
                    {
                        CommonOutPut("%s", "\n   ");
                    }                    
                    CommonOutPut(" %s (%s)", t_ip.c_str(), t_ip.c_str());
                }  
                int duration = (std::chrono::duration_cast<std::chrono::microseconds>(end_pt - start_pt)).count();
                CommonOutPut("  %0.2fms", duration / (float)1000);                          
            }
        }
        CommonOutPut("%s", "\n");
        return 0;
    }

    void open_ping_cmd(int index, string& str_result)
    {
        char buf[1024] = { 0 };
        sprintf(buf, __fmt_ping_order.c_str(), index);
        FILE* pp = popen(buf, "r");
        if(!pp)
        {
            LOGE("open %s fail", buf);
        }
        char line[1024] = { 0 };
        while (fgets(line, sizeof(line), pp) != NULL)
        {
            str_result += line;
            memset(line, 0, sizeof(line));         
        }
        LOGI("%s", str_result.c_str());
        pclose(pp); 
    }
    //is timeout
    bool process_ping_result(string& result, string& ip)
    {
        //_qyinfo(result);
        string::size_type pos = result.find("From ");
        if(std::string::npos != pos)
        {
            string ip_str = result.substr(pos + 5);
            //_qyerro(ip_str);
            string::size_type space_pos = ip_str.find(": ");
            if(std::string::npos != space_pos)
            {
                ip = ip_str.substr(0, space_pos);
                return false;
            }
        }
        return true;
    }
    vector<string> _result;
    shared_ptr<result_output> _output;
    string _host;
    string __fmt_ping_order;
    string __all_result;
    string __cur_ip;
    
};

#endif
