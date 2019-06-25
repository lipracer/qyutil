#include "../ping/pinger.hpp"
#include "../dnsquery/dnsquery.h"

extern "C"{

void ping(char* host)
{
    pinger<> pinger(host, 10, 0, 1, 1);
}
    
void TestDNSQuery(char* host, char* dnsSer)
{
    socket_ipinfo_t _ipinfo;
    _ipinfo.size = 0;
    socket_gethostbyname(host, &_ipinfo, 3000, dnsSer);
}
    
}
