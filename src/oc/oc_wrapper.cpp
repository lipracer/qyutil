#include "oc_wrapper.h"
#include "qylog.h"
#include "qyutil.h"

#include "../ping/pinger.hpp"
#include "../dnsquery/dnsquery.h"
#include "../tracerouter/tracerouter.hpp"

class OC_Callback : public QyUtil::Callback
{
public:
    OC_Callback(PingCallback cb) : oc_cb(cb){}
    void dns_query(vector<string>& vec, QYErrorInfo error)
    {

    }
    void ping(PingStatus& state, QYErrorInfo error)
    {
        if(oc_cb)
        {
            OC_PingStatus oc_statue;
            ZeroPingStatus(&oc_statue);
            oc_statue.loss_rate = state.loss_rate;
            oc_statue.minrtt = state.minrtt;
            oc_statue.avgrtt = state.avgrtt;  // ms
            oc_statue.maxrtt = state.maxrtt;  // ms
            strncpy(oc_statue.ip, state.ip, MAX_IP_BUF_LEN - 1);
            (*oc_cb)(&oc_statue, error->error_code, error->msg);
        }
    }
private:
    PingCallback oc_cb;
};

extern "C" {

void ZeroPingStatus(struct OC_PingStatus *status)
{
    status->loss_rate = 0.0;
    status->minrtt = 0.0;
    status->avgrtt = 0.0;
    status->maxrtt = 0.0;
    memset(status, 0, MAX_IP_BUF_LEN);
}

void OC_NetworkDiagnosis(const char* host, const char* dnsSer, PingCallback cb)
{
    auto _callback = make_shared<OC_Callback>(cb);
    string _host(host);
    string _dnsSer(dnsSer == nullptr ? "" : dnsSer);
    function<int(void)> fun_1 = std::bind(QyUtil::NetworkDiagnosis, _host, _dnsSer, 10000, _callback);
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
    //shared_ptr<result_output> output = make_shared<result_output>();
    pinger<1, __PLATFORM__> pinger(host, times, package_size, interval, timeout);
    //strcpy(result, output->result());
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
