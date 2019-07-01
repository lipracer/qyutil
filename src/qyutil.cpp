#include "qyutil.h"
#include <chrono>
#include <iostream>

#include "ping/pinger.hpp"
#include "dnsquery/dnsquery.h"
#include "tracerouter/tracerouter.hpp"

using namespace QyUtil;
using namespace std;

int NetworkDiagnosis(string HostName, string dnsServer)
{
    LOGI("call:%s host:%s", "NetworkDiagnosis", HostName.c_str());
    int ret = 0;
    socket_ipinfo_t ipInfo;
    ipInfo.size = 0;
    ret = socket_gethostbyname(HostName.c_str(), &ipInfo,  1000/*ms*/, dnsServer.c_str());//
    if(ret !=0 )
    {
        LOGE("query:%s fail!!!", HostName.c_str());
        return ret;
    }
    for (size_t i = 0; i < ipInfo.size; i++)
    {
#ifndef __ANDROID__
#define __ANDROID__ (0)
#endif

        pinger<1, __ANDROID__> pinger(ipInfo.ip[i].c_str(), 4, 0, 1, 0);
        TraceRouter<TraceRouterType::UDP, __ANDROID__> tr(ipInfo.ip[i]);
    }
    return ret;    
}
