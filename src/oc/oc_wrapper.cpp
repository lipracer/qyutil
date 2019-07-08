#include "oc_wrapper.h"
#include "qylog.h"
#include "qyutil.h"

#include "../ping/pinger.hpp"
#include "../dnsquery/dnsquery.h"
#include "../tracerouter/tracerouter.hpp"

extern "C" {

void OC_NetworkDiagnosis(const char* host, const char* dnsSer)
{
    function<int(void)> fun_1 = std::bind(QyUtil::NetworkDiagnosis, string(host), string(dnsSer), 1000);
    decltype(fun_1) fun_2 = [=]()->int{
        string _host = host;
        return 0;
    };
    auto task = make_pair(fun_1, fun_2);
    qyutil_instance.put_task(task);
}

int NativeGetHostNameByServer(const char* HostName, int timeout, const char* dnsServer, char* ipList)
{
    socket_ipinfo_t ipInfo;
    ipInfo.size = 0;
    int ret = socket_gethostbyname(HostName, &ipInfo,  1000/*ms*/, dnsServer);//
    string ips;
    if(0 == ret)
    {
        ips.clear();
        for(int i = 0; i < ipInfo.size; ++i)
        {
            ips += ipInfo.ip[i] + ",";
        }
        ips.pop_back();
        strcpy(ipList, ips.c_str());
    }
    return ret;
}

//sync call
int NativePing(const char* host, int times, int package_size, int interval/*S*/, int timeout/*S*/, char* result)
{
    shared_ptr<result_output> output = make_shared<result_output>();
    pinger<1, __PLATFORM__> pinger(host, times, package_size, interval, timeout, output);
    strcpy(result, output->result());
    return 0;
}

//sync call
int NativeTraceRouter(const char* host, char* result)
{
    shared_ptr<result_output> output = make_shared<result_output>();
    TraceRouter<TraceRouterType::UDP, __PLATFORM__> tr(host, output);
    strcpy(result, output->result());
    return 0;
}

static const int MaxStrBuf = 4096;

int AsyncNativeGetHostNameByServer(const char* HostName, int timeout, const char* dnsServer, AsynCB cb)
{
    shared_ptr<string> result = make_shared<string>();
    result->resize(MaxStrBuf);
    function<int(void)> fun_1 = std::bind(NativeGetHostNameByServer, HostName, timeout, dnsServer, const_cast<char*>(result->c_str()));
    decltype(fun_1) fun_2 = [=]()->int{
        if(cb)
        {
            cb(true, result->c_str());
        }
        return 0;
    };
    auto task = make_pair(fun_1, fun_2);
    qyutil_instance.put_task(task);
    return 0;
}

//sync call
int AsyncNativePing(const char* host, int times, int package_size, int interval/*S*/, int timeout/*S*/, AsynCB cb)
{
    shared_ptr<string> result = make_shared<string>();
    result->resize(MaxStrBuf);
    function<int(void)> fun_1 = std::bind(NativePing, host, times, package_size, interval, timeout, const_cast<char*>(result->c_str()));
    decltype(fun_1) fun_2 = [=]()->int{
        if(cb)
        {
            cb(true, result->c_str());
        }
        return 0;
    };
    auto task = make_pair(fun_1, fun_2);
    qyutil_instance.put_task(task);
    return 0;
}

//sync call
int AsyncNativeTraceRouter(const char* host, AsynCB cb)
{
    shared_ptr<string> result = make_shared<string>();
    result->resize(MaxStrBuf);
    function<int(void)> fun_1 = std::bind(NativeTraceRouter, host, const_cast<char*>(result->c_str()));
    decltype(fun_1) fun_2 = [=]()->int{
        if(cb)
        {
            cb(true, result->c_str());
        }
        return 0;
    };
    auto task = make_pair(fun_1, fun_2);
    qyutil_instance.put_task(task);
    return 0;
}
    
}
