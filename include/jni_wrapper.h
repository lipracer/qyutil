#include <jni.h>
#include <android/log.h>

 #define TAG "QYUtil-Log:" // 这个是自定义的LOG的标识  
 #define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__) // 定义LOGD类型   
 #define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__) // 定义LOGI类型  
 #define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__) // 定义LOGW类型  
 #define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__) // 定义LOGE类型   
 #define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, TAG, __VA_ARGS__) // 定义LOGF类型 

#ifdef __cplusplus
extern "C" {
#endif

//JNIEXPORT jint JNICALL Java_com_example_tqyutil_MainActivity_pingTest(JNIEnv *env, jobject obj);

#ifdef __cplusplus
}
#endif