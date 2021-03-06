#ifndef _QY_UTIL_NET_COMMON_H_
#define _QY_UTIL_NET_COMMON_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cmath>

#include <functional>
#include <string>
#include <sstream>

#include "qylog.h"

//#include <asio/ip/address_v4.hpp>
using namespace std;

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef long long int  int64;
typedef unsigned long long int u64;

namespace NetCommon
{
const string HTTP_HEADER_BREAK = "\r\n";
const string HTTP_HEADER_EOF = "\r\n\r\n";
const string HTTP_KET_CONTENT_LENGTH = "Content-Length";

enum class TraceRouterType : char
{
    UDP = 17,
    ICMP
};

const int MAX_MSG_BUF = 512;

static inline unsigned short __getpid()
{
    return static_cast<unsigned short>(::getpid());
}

//IP协议头
struct PROP_IP_HEADER
{
    char  header_len : 4;
    char  version : 4;          //版本信息(前4位)，头长度(后4位)
    char  type_of_service;     // 服务类型8位
    short pack_len;            //数据包长度
    short pack_id;             //数据包标识
    short dont_fragment  : 3;   //分片使用
    short more_fragments : 13;
    char  ttl;                 //存活时间
    char  protocol;             //协议类型
    short check_sum;           //校验和
    int   sour_ip;              //源ip
    int   dest_ip;              //目的ip

    PROP_IP_HEADER() :
    version(0XF & 4),
    header_len(0XF & 5),
    type_of_service(0),
    pack_len(PROP_IP_HEADER::Length),
    pack_id(0),
    dont_fragment(0),
    more_fragments(0),
    ttl(0),
    protocol((char)TraceRouterType::UDP),
    check_sum(0),
    sour_ip(0),
    dest_ip(0)
    { 
        pack_id = htons(0x96e2);//__getpid();
    }
    static const size_t Length = 20;
};

static_assert(sizeof(PROP_IP_HEADER) == PROP_IP_HEADER::Length, "PROP_IP_HEADER length error!!!");
//static_assert(std::is_pod<PROP_IP_HEADER>::value, "PROP_IP_HEADER is not pod type");

//UDP 协议头
struct alignas(2) UDP_HAEDER
{
    unsigned short sour_port; //源端口
    unsigned short dest_port; //目标端口
    unsigned short udp_len;
    unsigned short check_sum;
    UDP_HAEDER() : udp_len(UDP_HAEDER::Length){}   
    static const size_t Length = 8; 
};

static_assert(sizeof(UDP_HAEDER) == UDP_HAEDER::Length, "UDP_HAEDER length error!!!");
//static_assert(std::is_pod<UDP_HAEDER>::value, "UDP_HAEDER is not pod type");

typedef int RawSocket;

inline void socket_close(RawSocket sock) { close(sock); } //window 与 linux close不同

};
static const int OutputBufLen = 1024;
    
class result_output
{
public:
    result_output();
    virtual ~result_output();
    virtual void operator()(const char* fmt, ...);
    const char* result();
    
protected:
    char *_buf;
    stringstream _ss;
};

struct PingStatus {
    std::string res;
    double loss_rate;
    double minrtt;  // ms
    double avgrtt;  // ms
    double maxrtt;  // ms
    char ip[16];
    PingStatus() : loss_rate(0.0), minrtt(0.0), avgrtt(0.0), maxrtt(0.0) 
    {
        res = "";
        memset(ip, 0, sizeof(ip));
    }
    friend std::ostream& operator<<(std::ostream& os, PingStatus& status)
    {
        os << "PingStatus---->" << "loss_rate:" << status.loss_rate << " ";
        os << "minrtt:" << status.minrtt << " ";
        os << "avgrtt:" << status.avgrtt << " ";
        os << "maxrtt:" << status.maxrtt << " ";
        os << "ip:" << status.ip;
        return os;
    }
};

struct QYErrorInfo_
{
    int error_code;
    char msg[NetCommon::MAX_MSG_BUF];
    QYErrorInfo_()
    {
        recover();
    }
    QYErrorInfo_(int nerr, string& msg_) : QYErrorInfo_()
    {
        set(nerr, msg_.c_str());
    }
    void set(int nerr, const char* msg_)
    {
        error_code = nerr;
        if(msg_)
        {
            size_t size = strlen(msg_) >= NetCommon::MAX_MSG_BUF ? NetCommon::MAX_MSG_BUF-1 : strlen(msg_);
            strncpy(msg, msg_, size);
        }
        else
        {
            memset(msg, 0, NetCommon::MAX_MSG_BUF);
        }
    }
    void recover()
    {
        set(0, nullptr);
    }
    ~QYErrorInfo_()
    {
        LOGD("QYErrorInfo_ destruct!");
    }
};

using QYErrorInfo = shared_ptr<QYErrorInfo_>;
    
//static SimpleHttp __shttp;

#define CommonOutPut(...) do{\
if(_output) (*_output)(__VA_ARGS__);\
}\
while(false);

inline timeval dtime2timeval(double time)
{
    timeval result;
    memset(&result, 0, sizeof(result));
    result.tv_sec = floor(time);
    result.tv_usec = (time - result.tv_sec) * 1000 * 1000;
    return result;
}

#endif
