#include <jni.h>
#include <android/log.h>
#include <string>
#include "jni_wrapper.h"
#include "../ping/pinger.hpp"
#include "../dnsquery/dnsquery.h"
#include "../tracerouter/tracerouter.hpp"
#include "qyutil.h"

class jni_string
{
public:
    jni_string(JNIEnv *env, jstring jstr) : _env(env), _jstr(jstr)
    {
        if(_jstr)
        {
            _cstr = _env->GetStringUTFChars(jstr, 0);
            _str = _cstr;
        }
        else
        {
            _cstr = nullptr;
            _str = "";
        }
    }
    ~jni_string()
    {
        if(_jstr) _env->ReleaseStringUTFChars(_jstr, _cstr);
    }
    string& str()
    {
        return _str;
    }
protected:
    const char *_cstr;
    string _str;
    jstring _jstr;
    JNIEnv *_env;
};

jstring chartoJstring(JNIEnv *env, const char *pat) {
    jclass strClass = (env)->FindClass("java/lang/String");
    jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = (env)->NewByteArray((jsize) strlen(pat));
    (env)->SetByteArrayRegion(bytes, 0, (jsize) strlen(pat), (jbyte *) pat);
    jstring encoding = (env)->NewStringUTF("utf-8");
    return (jstring) (env)->NewObject(strClass, ctorID, bytes, encoding);
}

#ifdef __cplusplus
extern "C" {
#endif

int _GetHostNameByServer(string& HostName, list<string>& dnsServer);
int _Ping(string& host, int times, int package_size, int interval/*S*/, int timeout/*S*/, string& result);
int _TraceRouter(string& host, string& result);

DEFJNIFUNC(jstring, NativeQueryDNS, jstring host, jstring dnsSer, jint timeout/*S*/)
{
    jni_string jstrh(env, host);
    jni_string jstrd(env, dnsSer);
    socket_ipinfo_t ipInfo;
    ipInfo.size = 0;
    int ret = socket_gethostbyname(jstrh.str().c_str(), &ipInfo,  timeout/*ms*/, jstrd.str().c_str());//
    string result;
    if(0 == ret)
    {
        for(int i = 0; i < ipInfo.size; ++i)
        {
            result = result + ipInfo.ip[i] + ","; 
        }
        result.pop_back();
    }
    return chartoJstring(env, result.c_str());
}

DEFJNIFUNC(jstring, NativePing, jstring host, jint times, jint package_size, jint interval/*S*/, jint timeout/*S*/)
{
    jni_string jstr(env, host);
    shared_ptr<result_output> output = make_shared<result_output>();
    string host_ = jstr.str();
    pinger<1, __PLATFORM__> pinger(host_, (int)times, (int)package_size, (int)interval, (int)timeout, output);
    const char* result = output->result(); 
    return chartoJstring(env, result);
}

DEFJNIFUNC(jstring, NativeTraceRouter, jstring host)
{
    jni_string jstr(env, host);
    shared_ptr<result_output> output = make_shared<result_output>();
    TraceRouter<TraceRouterType::UDP, __PLATFORM__> tr(jstr.str(), output);
    const char* result = output->result(); 
    return chartoJstring(env, result);
}

DEFJNIFUNC(int, NetworkDiagnosis, jstring host) 
{
    LOGI("%s", "call jni func TestDNSQuery");
    jni_string jstr(env, host);
    function<int(void)> fun_1 = std::bind(QyUtil::NetworkDiagnosis, std::string(jstr.str()), std::string("10.16.169.127"), 1000);
    decltype(fun_1) fun_2 = nullptr;
    auto task = make_pair(fun_1, fun_2);
    QyUtil::qyutil<1>::getInstance().put_task(task); 
    return 0;
}  

#ifdef __cplusplus
}
#endif