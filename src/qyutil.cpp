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
    shared_ptr<result_output> output = make_shared<result_output>();
    result_output& _output = *output;
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
    (*output)("dns query %s ip list:\n", HostName.c_str());
    for (size_t i = 0; i < ipInfo.size; i++)
    {
        _output("%s\n", ipInfo.ip[i].c_str());
    }
    for (size_t i = 0; i < ipInfo.size; i++)
    {
        pinger<1, __PLATFORM__> pinger(ipInfo.ip[i].c_str(), 4, 0, 1, 0, output);
        TraceRouter<TraceRouterType::UDP, __PLATFORM__> tr(ipInfo.ip[i], output);
    }
    return ret;    
}
