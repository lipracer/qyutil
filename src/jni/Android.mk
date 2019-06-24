LOCAL_PATH := $(call my-dir)
#MY_VAR:=XXX
#$(warning $(LOCAL_PATH))
include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cxx .cpp .cc .hpp
LOCAL_MODULE := qyutil
#NDK_OUT := ../../build/android
#NDK_APP_DST_DIR := ../../build/android/$(TARGET_ARCH_ABI) #指定so目录
LOCAL_MODULE_PATH := $(LOCAL_MODULE)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../common/boost/ $(LOCAL_PATH)/../../include
$(warning "LOCAL_C_INCLUDES:"$(LOCAL_C_INCLUDES))
LOCAL_SRC_FILES := jni_wrapper.cpp ../qyutil.cpp
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)
