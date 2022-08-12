#include <jni.h>
#include <string>

#include "dobby.h"
#include <android/log.h>
#include "logging/logging.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define TAG "dobby-helper-log"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__);
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG ,__VA_ARGS__);
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG ,__VA_ARGS__);
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG ,__VA_ARGS__);

size_t (*old_strlen)(const char *str);

static void *strlen_symbol = NULL;

static size_t new_strlen(const char *str) {
    if (strlen_symbol != NULL) {
        DobbyDestroy((void *) strlen_symbol);
    }

    if (str != NULL) {
        LOGD("---------- strlen：%s ", str);
    }
    size_t ret = old_strlen(str);

    if (strlen_symbol != NULL) {
        DobbyHook((void *) strlen_symbol, (void *) new_strlen, (void **) &old_strlen);
    }

    return ret;
}

extern "C" JNIEXPORT jstring
JNICALL Java_com_friday_dobby_MainActivity_stringFromJNI(JNIEnv *env, jobject /* this */) {
    if (strlen_symbol == NULL) {
        strlen_symbol = DobbySymbolResolver("libc.so", "strlen");
    }
    if (strlen_symbol != NULL) {
        DobbyHook((void *) strlen_symbol, (void *) new_strlen, (void **) &old_strlen);
    }

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}