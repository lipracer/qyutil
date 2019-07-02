#ifndef _QYUTIL_JNIWRAPPER_
#define _QYUTIL_JNIWRAPPER_

#include "qylog.h"

#define DEFJNIFUNC(ReturnType, Name, ...) \
JNIEXPORT ReturnType JNICALL Java_com_iqiyi_pizza_react_fragment_##Name(JNIEnv *env, jobject obj, __VA_ARGS__)

#endif