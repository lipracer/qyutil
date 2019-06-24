#include "../ping/pinger.hpp"

extern "C"{

void ping(char* host)
{
    pinger<> pinger(host, 10, 0, 1, 1);
}
    
}
