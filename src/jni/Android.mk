LOCAL_PATH := $(call my-dir)
#MY_VAR:=XXX
#$(warning $(LOCAL_PATH))
include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cxx .cpp .cc .hpp
LOCAL_MODULE := qyutil
#NDK_OUT := ../../build/android
#NDK_APP_DST_DIR := ../../build/android/$(TARGET_ARCH_ABI) #指定so目录
LOCAL_MODULE_PATH := $(LOCAL_MODULE)

LOCAL_CFLAGS += -DASIO_STANDALONE

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../common/asio/include $(LOCAL_PATH)/../../common $(LOCAL_PATH)/../../include

LOCAL_SRC_FILES += $(LOCAL_PATH)/../../common/qylog/qyLogMgr.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../common/qylog/QYLogger.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../dnsquery/dnsquery.cpp
LOCAL_SRC_FILES += jni_wrapper.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../qyutil.cpp


LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)
