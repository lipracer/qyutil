extern "C" {

typedef void(*AsynCB)(bool, const char*);

void OC_NetworkDiagnosis(const char* host, const char* dnsSer);

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
    
}
