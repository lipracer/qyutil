#ifndef _QYUTIL_JNIWRAPPER_
#define _QYUTIL_JNIWRAPPER_

#include "qylog.h"

#define DEFJNIFUNC(ReturnType, Name, ...) __DEFJNIFUNC(Java_com_iqiyi_inq_netutils, ProbeWrapper, ReturnType, Name, __VA_ARGS__)

#define __DEFJNIFUNC(FullPath, ClassName, ReturnType, Name, ...) \
JNIEXPORT ReturnType JNICALL FullPath##_##ClassName##_##Name(JNIEnv *env, jobject obj, __VA_ARGS__)

#endif