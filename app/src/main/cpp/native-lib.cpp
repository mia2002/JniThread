#ifndef _Nonnull
#define _Nonnull
#endif

#include <jni.h>
#include <string>
#include <pthread.h>
#include <android/log.h>
#include <malloc.h>

#define  LOG_TAG    "C_LOG"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

//Java虚拟机接口指针
static JavaVM* gVm = NULL;

static jobject gObj = NULL;

//缓存回调方法的methodID
static jmethodID gOnNativeResult = NULL;

/**
 * 使用JNI_OnLoad方法可以获取java虚拟机接口指针。
 * 当共享库开始加载时虚拟机会自动调用该方法。
 * @param vm
 * @param reserved
 * @return
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved){
    //缓存Java虚拟机接口指针
    gVm = vm;

    return JNI_VERSION_1_4;
}

static void* nativeThread(void* args){
    JNIEnv* env = NULL;
    if (0 == gVm->AttachCurrentThread(&env,NULL)){
        char* message = (char*) args;
        LOGI("native %s \n",message);
        jstring messageString = env->NewStringUTF(message);
        env->CallVoidMethod(gObj,gOnNativeResult,messageString);
        gVm->DetachCurrentThread();
    }
    return (void*) 1;
}

extern "C"
JNIEXPORT void JNICALL
Java_pub_yanng_jnithread_MainActivity_nativeInit(JNIEnv *env, jobject instance) {

//    //如果全局对象为空，则初始化
    if (NULL == gObj){
        //为对象创建一个新的全局引用
        gObj = env->NewGlobalRef(instance);
        if (NULL == gObj){
            return;
        }
    }

    //回调方法methodId初始化
    if (NULL == gOnNativeResult){
        //反射
        jclass clazz = env->GetObjectClass(instance);
        gOnNativeResult = env->GetMethodID(clazz,"onNativeResult","(Ljava/lang/String;)V");
        if (NULL == gOnNativeResult){
            //没有声明回调方法
            jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
            //抛出异常
            env->ThrowNew(exceptionClazz,"没有声明onNativeResult()方法");
            return;
        }
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_pub_yanng_jnithread_MainActivity_nativeFree(JNIEnv *env, jobject instance) {

    if (NULL != gObj){
        //删除引用
        env->DeleteGlobalRef(gObj);
        gObj = NULL;
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_pub_yanng_jnithread_MainActivity_posixThread(JNIEnv *env, jobject instance) {

    pthread_t thread;
    const char *message = "native thread";

    int result = pthread_create(&thread,NULL,nativeThread,(void*) message);
    if (0 != result){
        //获取异常类
        jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
        //抛出异常
        env->ThrowNew(exceptionClazz, "无法创建线程");
    }

    void* value = NULL;
    if(0 != pthread_join(thread,&value)){
        //获取异常
        jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
        //抛出异常
        env->ThrowNew(exceptionClazz,"unable to join thread");
        return;
    }

}

