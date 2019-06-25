#ifndef _QYUTIL_QYLOG_H_
#define _QYUTIL_QYLOG_H_

#include "../common/qylog/qyLogMgr.h"

static const char * TAG = "QYUtil-Log"; // 这个是自定义的LOG的标识  

#define LOGD(...) QYLogCWrapper(3, TAG, __VA_ARGS__) // 定义LOGD类型   
#define LOGI(...) QYLogCWrapper(4, TAG, __VA_ARGS__) // 定义LOGI类型  
#define LOGW(...) QYLogCWrapper(5, TAG, __VA_ARGS__) // 定义LOGW类型  
#define LOGE(...) QYLogCWrapper(6, TAG, __VA_ARGS__) // 定义LOGE类型   
#define LOGF(...) QYLogCWrapper(7, TAG, __VA_ARGS__) // 定义LOGF类型 

#endif