#ifndef _OC_WRAPPER_H_
#define _OC_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_IP_BUF_LEN (16)

struct OC_PingStatus 
{
    double loss_rate;
    double minrtt;  // ms
    double avgrtt;  // ms
    double maxrtt;  // ms
    char ip[MAX_IP_BUF_LEN];
};

typedef void(*AsynCB)(bool, const char*);
typedef void(*PingCallback)(const struct OC_PingStatus *status, int code, const char *msg);

void ZeroPingStatus(struct OC_PingStatus *status);

void OC_NetworkDiagnosis(const char* host, const char* dnsSer, PingCallback cb);

int NativeGetHostNameByServer(const char* HostName, int timeout, const char* dnsServer, char* ipList);

//sync call
int NativePing(const char* host, int times, int package_size, int interval/*S*/, int timeout/*S*/, char* result);

//sync call
int NativeTraceRouter(const char* host, char* result);



int AsyncNativeGetHostNameByServer(const char* HostName, int timeout, const char* dnsServer, AsynCB cb);

//sync call
int AsyncNativePing(const char* host, int times, int package_size, int interval/*S*/, int timeout/*S*/, AsynCB cb);

//sync call
int AsyncNativeTraceRouter(const char* host, AsynCB cb);

#ifdef __cplusplus 
}
#endif

#endif
