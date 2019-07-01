#ifndef _QY_UTIL_NET_COMMON_H_
#define _QY_UTIL_NET_COMMON_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <functional>
#include <string>
using namespace std;

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef long long int  int64;
typedef unsigned long long int u64;

namespace NetCommon
{
enum class TraceRouterType : char
{
    UDP = 17,
    ICMP
};

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

typedef int RawSocket;

inline void socket_close(RawSocket sock) { close(sock); } //window 与 linux close不同

};

//cpp string 转来转去太恶心
using CheckOutput = std::function<void(const char*)>;
void __DefauleOutput(const char* result);
static const CheckOutput DefauleOutPut = __DefauleOutput;
static const int OutputBufLen = 1024;

#endif
