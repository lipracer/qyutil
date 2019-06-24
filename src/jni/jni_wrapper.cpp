#include <jni.h>
#include <android/log.h>
#include <string>
#include "jni_wrapper.h"
#include "../ping/pinger.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#define DEF_JNI_W(Return, Nanme, ...) \
JNIEXPORT Return JNICALL Java_com_iqiyi_pizza_react_fragment_##Name(JNIEnv *env, jobject obj, __VA_ARGS__)

static void sync_ping(std::string host)
{
    LOGE("send---------------------------%s", host.c_str());
    pinger<1, __ANDROID__> ping(host, 10, 0, 1, 10);
}
//com.iqiyi.pizza.react.fragment.PZWelfareCenterFragment
                     //Java_com_iqiyi_pizza_react_fragment_PZWelfareCenterFragment_pingTest
JNIEXPORT jint JNICALL Java_com_iqiyi_pizza_react_fragment_PZWelfareCenterFragment_pingTest(JNIEnv *env, jobject obj, jstring host) 
{
    const char *host_ = env->GetStringUTFChars(host, 0);
    sync_ping(host_);
    env->ReleaseStringUTFChars(host, host_);
    return 0;
}  

#ifdef __cplusplus
}
#endif