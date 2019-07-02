#include <jni.h>
#include <android/log.h>
#include <string>
#include "jni_wrapper.h"
#include "../ping/pinger.hpp"
#include "../dnsquery/dnsquery.h"
#include "../tracerouter/tracerouter.hpp"
#include "qyutil.h"

#ifdef __cplusplus
extern "C" {
#endif

class jni_string
{
public:
    jni_string(JNIEnv *env, jstring jstr) : _env(env), _jstr(jstr)
    {
        _cstr = _env->GetStringUTFChars(jstr, 0);
    }
    ~jni_string()
    {
        _env->ReleaseStringUTFChars(_jstr, _cstr);
    }
    const char* c_str()
    {
        return _cstr;
    }
protected:
    const char* _cstr;
    jstring _jstr;
    JNIEnv *_env;
};

DEFJNIFUNC(int, NetworkDiagnosis, jstring host) 
{
    LOGI("%s", "call jni func TestDNSQuery");
    jni_string jstr(env, host);
    std::string host_ = jstr.c_str();
    function<int(void)> fun_1 = std::bind(NetworkDiagnosis, host_, "10.16.169.127");
    decltype(fun_1) fun_2 = nullptr;
    auto task = make_pair(fun_1, fun_2);
    QyUtil::qyutil<1>::getInstance().put_task(task); 
    return 0;
}  

#ifdef __cplusplus
}
#endif