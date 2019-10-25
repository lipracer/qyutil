#ifndef _QYUTIL_QYLOG_H_
#define _QYUTIL_QYLOG_H_

#include <sstream>
#include "../common/qylog/qyLogMgr.h"

static const char * TAG = "QYUtil-Log"; // 这个是自定义的LOG的标识

#define LOGD(...) QYLogCWrapper(3, TAG, __VA_ARGS__) // 定义LOGD类型   
#define LOGI(...) QYLogCWrapper(4, TAG, __VA_ARGS__) // 定义LOGI类型  
#define LOGW(...) QYLogCWrapper(5, TAG, __VA_ARGS__) // 定义LOGW类型  
#define LOGE(...) QYLogCWrapper(6, TAG, __VA_ARGS__) // 定义LOGE类型   
#define LOGF(...) QYLogCWrapper(7, TAG, __VA_ARGS__) // 定义LOGF类型

//template <typename T>
//void __CPP_LOG(int level, T arg)
//{
//
//}

template <typename ...Args>
void __CPP_LOG(int level, Args ...args)
{
//    return __CPP_LOG(level, args...);
    stringstream ss;
    int aa[] = { (ss << args, 0)... };
    QYLogCWrapper(level, TAG, "%s", ss.str().c_str());
}

//only for cpp
#define _qydebug(...) __CPP_LOG(3, __VA_ARGS__) // 定义LOGD类型
#define _qyinfo(...) __CPP_LOG(4, __VA_ARGS__) // 定义LOGI类型
#define _qywarn(...) __CPP_LOG(5, __VA_ARGS__) // 定义LOGW类型
#define _qyerro(...) __CPP_LOG(6, __VA_ARGS__) // 定义LOGE类型
#define _qyfalt(...) __CPP_LOG(7, __VA_ARGS__) // 定义LOGF类型

#endif
