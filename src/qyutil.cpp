#include "qyutil.h"
#include <chrono>
#include <iostream>
#include <string>

#include "ping/pinger.hpp"
#include "dnsquery/dnsquery.h"
#include "tracerouter/tracerouter.hpp"

using namespace std;

namespace qyutil{

int NetworkDiagnosis(string HostName, string dnsServer, int timeout, shared_ptr<Callback> cb)
{
    // shared_ptr<result_output> output = make_shared<result_output>();
    // result_output& _output = *output;
    LOGI("call:%s host:%s", "NetworkDiagnosis", HostName.c_str());
    int ret = 0;
    socket_ipinfo_t ipInfo;
    ipInfo.size = 0;
    ret = socket_gethostbyname(HostName.c_str(), &ipInfo,  timeout/*ms*/, dnsServer.c_str());//
    if(ret !=0 )
    {
        LOGE("query:%s fail!!!", HostName.c_str());
    }
    // (*output)("dns query %s ip list:\n", HostName.c_str());

    vector<string> vec_ip;
    for (size_t i = 0; i < ipInfo.size; i++)
    {
        // _output("%s\n", ipInfo.ip[i].c_str());
        vec_ip.push_back(ipInfo.ip[i]);
    }
    auto error = make_shared<QYErrorInfo_>();
    if(0 == ipInfo.size)
    {
        error->set(-1, "unknown host");
    }
    cb->dns_query(vec_ip, error);
    _qyinfo("start ping!");
    if(0 == ipInfo.size)
    {
        error->recover();
        error->set(-1, "empty ip list");
        PingStatus status; 
        cb->ping(status, error);
    }
    for (size_t i = 0; i < ipInfo.size; i++)
    {
        pinger<1, __PLATFORM__> pinger(ipInfo.ip[i].c_str(), 4, 0, 1, 0);
        //TraceRouter<TraceRouterType::UDP, __PLATFORM__> tr(ipInfo.ip[i], output);
#if 1
        stringstream ss;
        ss << pinger._ping_status;
        _qyinfo(ss.str());
#endif
        error->recover();
        cb->ping(pinger._ping_status, error);
    }
    return ret;    
}
#if 0
int _GetHostNameByServer(string HostName, int timeout, string dnsServer, list<string>& ipList)
{
    socket_ipinfo_t ipInfo;
    ipInfo.size = 0;
    int ret = socket_gethostbyname(HostName.c_str(), &ipInfo,  1000/*ms*/, dnsServer.c_str());//
    if(0 == ret)
        for(int i = 0; i < ipInfo.size; ++i)
            ipList.push_back(ipInfo.ip[i]);

    return ret;
}

//sync call
int _Ping(string host, int times, int package_size, int interval/*S*/, int timeout/*S*/, string& result)
{
    shared_ptr<result_output> output = make_shared<result_output>();
    pinger<1, __PLATFORM__> pinger(host, times, package_size, interval, timeout, output);
    result.clear();
    result = output->result();    
    return 0;
}

//sync call
int _TraceRouter(string host, string& result)
{
    shared_ptr<result_output> output = make_shared<result_output>();
    TraceRouter<TraceRouterType::UDP, __PLATFORM__> tr(host, output);
    result.clear();
    result = output->result(); 
    return 0;
}
#endif

}
