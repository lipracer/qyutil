#include <jni.h>
#include <android/log.h>
#include <vector>
#include <string>
#include <memory>
#include "jni_wrapper.h"
#include "../ping/pinger.hpp"
#include "../dnsquery/dnsquery.h"
#include "../tracerouter/tracerouter.hpp"
#include "qyutil.h"

static JavaVM *jvm = nullptr;

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

#define CHECK_ENV_FUNC_RESULT(result) if(nullptr==result){ret=JNI_ERR;break;}
template<typename T>
int vector2arraylist(JNIEnv *env, vector<T>& vec, jobject *arraylist)
{
    int ret = 0;
    do{
        jclass class_array_list = env->FindClass("java/util/ArrayList");
        if(nullptr == class_array_list){
            ret = JNI_ERR;
            LOGE("FindClass Error!");
            break;
        }
        jmethodID list_init = env->GetMethodID(class_array_list, "<init>", "()V");
        CHECK_ENV_FUNC_RESULT(list_init);
        jobject list_obj = env->NewObject(class_array_list, list_init);
        CHECK_ENV_FUNC_RESULT(list_obj);
        jmethodID list_add = env->GetMethodID(class_array_list, "add", "(Ljava/lang/Object;)Z");
        CHECK_ENV_FUNC_RESULT(list_add);
        for(auto& it : vec)
        {
            env->CallBooleanMethod(list_obj, list_add, env->NewStringUTF(it.c_str()));
        }   
        *arraylist = list_obj;  

    }while(false);
    if(ret) LOGE("vector2arraylist Error!");
    return ret;
}

class JNI_Callback : public QyUtil::Callback
{
public:
    //传入引用 防止虚拟机释放
    JNI_Callback(jobject jcallback) : _jcallback(jcallback), _env(nullptr)
    {

    }
    ~JNI_Callback()
    {

    }
    void init_env()
    {
        if(jvm->GetEnv((void**)&_env, JNI_VERSION_1_4) != JNI_OK){
            LOGE("GetEnv Error!");
            _env = nullptr;
        }
        _class_callback = _env->GetObjectClass(_jcallback);
    }
    void dns_query(vector<string>& vec)
    {
       int ret = 0;
        do{
            jmethodID method_id_dnsquery = _env->GetMethodID(_class_callback, "DnsQuery", "(Ljava/util/ArrayList;)V");
            if(nullptr == method_id_dnsquery)
            {
                ret = JNI_ERR;
                LOGE("GetMethodID Error!");
                break;
            }
            jobject result = nullptr;
            vector2arraylist<string>(_env, vec, &result);
 
            _env->CallVoidMethod(_jcallback, method_id_dnsquery, result);
        }while(false);
    }
    void ping(PingStatus state)
    {
        const char *signPingFunc = "(DDDDLjava/lang/String;)V";
        jmethodID method_id_ping = _env->GetMethodID(_class_callback, "ping", signPingFunc);
        _env->CallVoidMethod(_jcallback, method_id_ping, state.loss_rate, state.minrtt, state.avgrtt, state.maxrtt, _env->NewStringUTF("xxxx"));
    }
private:
    jobject _jcallback;
    JNIEnv *_env;
    jclass _class_callback;
    int get_jping_status(jobject *jps, PingStatus& status)
    {
        return 0;
    }
public:
    //jclass class_ping_status;
};

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

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("%s", "JNI_OnLoad");
    jvm = vm;
    return JNI_VERSION_1_4;
}
/*
c++回调Java :
1.创建引用，防止虚拟机把回调接口释放
2.拿到回调对象的类方法id
3.调用类方法
4.
*/
DEFJNIFUNC(int, NetworkDiagnosis, jstring host, jobject callback) 
{
    LOGI("%s", "call jni func TestDNSQuery");
    jni_string jstr(env, host);
    string _host = std::string(jstr.str());
    jobject ref_callback = env->NewGlobalRef(callback);

    auto _callback = make_shared<JNI_Callback>(ref_callback);
    //jclass class_ping_status = env->FindClass("com/iqiyi/networkutil/NetworkUtil$PingStatus");
    //_callback->class_ping_status = class_ping_status;
    //LOGE("find status class:%d", class_ping_status);
    function<int(void)> run_task = [=]()->int{
        if(jvm) {
            if (jvm->AttachCurrentThread(const_cast<JNIEnv**>(&env), nullptr))//将当前线程注册到虚拟机中
            {
                LOGE("AttachCurrentThread Fail!!!");
                return -1;
            }
            _callback->init_env();
            QyUtil::NetworkDiagnosis(_host, std::string("10.16.169.127"), 1000, _callback);
        }
        return 0;
    };
    auto task = make_pair(run_task, nullptr);

    QyUtil::qyutil<1>::getInstance().put_task(task); 
    return 0;
}  

#ifdef __cplusplus
}
#endif