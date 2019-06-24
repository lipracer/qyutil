#ifndef _QYLOG_H_
#define _QYLOG_H_

#if __ANDROID__
#include "jni_wrapper.h"
#else
#include <stdio.h>

 #define TAG "QYUtil-Log:" // 这个是自定义的LOG的标识  
 #define LOGD(...) printf(__VA_ARGS__)
 #define LOGI(...) printf(__VA_ARGS__)
 #define LOGW(...) printf(__VA_ARGS__)
 #define LOGE(...) printf(__VA_ARGS__)
 #define LOGF(...) printf(__VA_ARGS__) // 定义LOGF类型 

#endif

#endif