#include <jni.h>
#include <android/log.h>
#include <string>
#include "jni_wrapper.h"
#include "../ping/pinger.hpp"
#include "../dnsquery/dnsquery.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEF_JNI_W(Return, Nanme, ...) \
JNIEXPORT Return JNICALL Java_com_iqiyi_pizza_react_fragment_##Name(JNIEnv *env, jobject obj, __VA_ARGS__)

static void test_dns(std::string host)
{
    socket_ipinfo_t _ipinfo;
    _ipinfo.size = 0;
    socket_gethostbyname(host.c_str(), &_ipinfo, 3000, "");
    
    for(int i = 0; i < _ipinfo.size ; ++i)
    {
        LOGD("query ip:%s", _ipinfo.ip[i].c_str());
        pinger<1, __ANDROID__> pinger(_ipinfo.ip[i].c_str(), 10, 0, 1, 0);
    }
}
//com.iqiyi.pizza.react.fragment.PZWelfareCenterFragment
                     //Java_com_iqiyi_pizza_react_fragment_PZWelfareCenterFragment_pingTest
JNIEXPORT jint JNICALL Java_com_iqiyi_pizza_react_fragment_PZWelfareCenterFragment_TestDNSQuery(JNIEnv *env, jobject obj, jstring host) 
{
    const char *host_ = env->GetStringUTFChars(host, 0);
    test_dns(host_);
    env->ReleaseStringUTFChars(host, host_);
    return 0;
}  

#ifdef __cplusplus
}
#endif