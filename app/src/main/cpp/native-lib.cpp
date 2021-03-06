#include <jni.h>
#include <android/log.h>
#include <cstdio>


#define TAG "native-lib"
#include "memtester/memtester.h"
extern "C" {

    int main(int argc, char **argv);

}

jmethodID jMtdOnTestStart;
jmethodID jMtdOnTestProgress;
jmethodID jMtdOnTestCompleted;
jmethodID jMtdOnMessage;
JNIEnv *g_env;
jobject g_instance;

extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    __android_log_print(ANDROID_LOG_VERBOSE, TAG, "JNI_OnLoad");
    JNIEnv* env;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR; // JNI version not supported.
    }
    jclass clz = env->FindClass("com/haoyun/memtester/MemTester");
    jMtdOnTestStart = env->GetMethodID(clz, "onTestStart", "(ILjava/lang/String;)V");
    jMtdOnTestProgress = env->GetMethodID(clz, "onTestProgress", "(IF)V");
    jMtdOnTestCompleted = env->GetMethodID(clz, "onTestCompleted", "(II)V");
    jMtdOnMessage = env->GetMethodID(clz, "onMessage", "(ILjava/lang/String;)V");
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_haoyun_memtester_MemTester_native_1start(JNIEnv *env, jobject instance, jint size,
                                                  jint loop) {
    g_env = env;
    g_instance = instance;
    char arg_size[10] = {0};
    char arg_loop[10] = {0};
    sprintf(arg_size, "%dM", size);
    sprintf(arg_loop, "%d", loop);
    char *argv[] = {"./memtester", arg_size, arg_loop};
    main(3, argv);
}

extern "C"
void onTestStart(int index, char* name) {
    __android_log_print(ANDROID_LOG_INFO, TAG, "onTestStart: %d, %s", index, name);
    jint javaIndex= index;
    jstring javaName = g_env->NewStringUTF(name);
    g_env->CallVoidMethod(g_instance, jMtdOnTestStart, javaIndex, javaName);
}

extern "C"
void onTestProgress(int index, float progress) {
    __android_log_print(ANDROID_LOG_VERBOSE, TAG, "onTestProgress: %d, %f", index, progress);
    g_env->CallVoidMethod(g_instance, jMtdOnTestProgress, index, progress);
}

extern "C"
void onTestCompleted(int index, int result) {
    __android_log_print(ANDROID_LOG_INFO, TAG, "onTestCompleted: %d, %d", index, result);
    g_env->CallVoidMethod(g_instance, jMtdOnTestCompleted, index, result);
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_haoyun_memtester_MemTester_native_1get_1tests(JNIEnv *env, jobject instance) {
    jsize count = TESTS_SIZE - 1; // last element is NULL
    jobjectArray javaStringArray;
    javaStringArray = env->NewObjectArray(count, env->FindClass("java/lang/String"), env->NewStringUTF(""));
    for (int i = 0; i < count; i++) {
        env->SetObjectArrayElement(javaStringArray, i, env->NewStringUTF((tests[i].name)));
    }
    return javaStringArray;
}

extern "C"
void onMessage(int priority, char* message) {
    __android_log_print(ANDROID_LOG_VERBOSE, TAG, "onMessage: %d, %s", priority, message);
    jint javaPriority= priority;
    jstring javaMessage = g_env->NewStringUTF(message);
    g_env->CallVoidMethod(g_instance, jMtdOnMessage, javaPriority, javaMessage);
}

