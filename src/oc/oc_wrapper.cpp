#include "../ping/pinger.hpp"
#include "../dnsquery/dnsquery.h"
#include "../../include/qylog.h"

void ping(char* host)
{
    pinger<> pinger(host, 10, 0, 1, 1);
}    


void TestDNSQuery(char* host, char* dnsSer)
{
    socket_ipinfo_t _ipinfo;
    _ipinfo.size = 0;
    socket_gethostbyname(host, &_ipinfo, 3000, dnsSer);
    
    for(int i = 0; i < _ipinfo.size ; ++i)
    {
        LOGD("query ip:%s", _ipinfo.ip[i].c_str());
        pinger<> pinger(_ipinfo.ip[i].c_str(), 10, 0, 1, 1);
    }
}
