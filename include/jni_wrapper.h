#ifndef _QYUTIL_JNIWRAPPER_
#define _QYUTIL_JNIWRAPPER_

#include "qylog.h"

//#define DEFJNIFUNC(ReturnType, Name, ...) __DEFJNIFUNC(Java_com_iqiyi_inq_netutils, ProbeWrapper, ReturnType, Name, __VA_ARGS__)

#define DEFJNIFUNC(ReturnType, Name, ...) \
JNIEXPORT ReturnType JNICALL Java_com_iqiyi_inq_netutils##_##ProbeWrapper##_##Name(JNIEnv *env, jclass jcls, __VA_ARGS__)

#endif