#include <jni.h>
#include <string>
#include "operations.h"

#define APPNAME "asdf"

#define C_FUN(RETURN, METHOD_NAME) extern "C" JNIEXPORT RETURN JNICALL Java_cz_applifting_convolutioncamera_MainActivity_##METHOD_NAME


C_FUN(jstring, jniGetString)(JNIEnv *env, jobject) {
    char *hello = const_cast<char *>("Hello from C++");
    return env->NewStringUTF(hello);
}

C_FUN(jint, jniCompute)(JNIEnv *env, jobject instance, jobject input) {

    jbyte *elements = static_cast<jbyte *>(env->GetDirectBufferAddress(input));
    jlong size = env->GetDirectBufferCapacity(input);

    for (int i = 0; i < size; i++) {
        elements[i] += 5;
    }

    return 0;
}

C_FUN(jobject, jniConvertBitmap)(JNIEnv *env, jobject instance, jobject bitmap, jint operation) {

    // Call method from NDK
    jstring jstr = env->NewStringUTF("This comes from jni.");
    jclass activityClass = env->FindClass("cz/applifting/convolutioncamera/MainActivity");
    jmethodID myMethod = env->GetMethodID(activityClass, "myMethodFromJni", "(Ljava/lang/String;)I");

    int result = env->CallIntMethod(instance, myMethod, jstr);

    //
    //getting bitmap info:
    //
    LOGD("reading bitmap info...");
    AndroidBitmapInfo info;
    int ret;
    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return nullptr;
    }

    LOGD("width:%d height:%d stride:%d", info.width, info.height, info.stride);
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("Bitmap format is not RGBA_8888!");
        return nullptr;
    }

    //
    //read pixels of bitmap into native memory :
    //
    LOGD("reading bitmap pixels...");
    void *bitmapPixels;
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels()1 failed ! error=%d", ret);
        return nullptr;
    }

    uint32_t *src = (uint32_t *) bitmapPixels;
    uint32_t *tempPixels = new uint32_t[info.height * info.width];
    int stride = info.stride;
    int pixelsCount = info.height * info.width;
    memcpy(tempPixels, src, sizeof(uint32_t) * pixelsCount);
    AndroidBitmap_unlockPixels(env, bitmap);

    //
    //recycle bitmap - using bitmap.recycle()
    //
    LOGD("recycling bitmap...");
    jclass bitmapCls = env->GetObjectClass(bitmap);
    jmethodID recycleFunction = env->GetMethodID(bitmapCls, "recycle", "()V");
    if (recycleFunction == nullptr) {
        LOGE("error recycling!");
        return nullptr;
    }
    env->CallVoidMethod(bitmap, recycleFunction);

    //
    //creating a new bitmap to put the pixels into it - using Bitmap Bitmap.createBitmap (int width, int height, Bitmap.Config config) :
    //
    LOGD("creating new bitmap...");
    jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls,
                                                            "createBitmap",
                                                            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jstring configName = env->NewStringUTF("ARGB_8888");
    jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");

    jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(bitmapConfigClass,
                                                                   "valueOf",
                                                                   "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");

    jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass,
                                                       valueOfBitmapConfigFunction,
                                                       configName);

    uint32_t width;
    uint32_t height;

    if (operation == 0) {
        width = info.height;
        height = info.width;
    } else {
        width = info.width;
        height = info.height;
    }

    jobject newBitmap = env->CallStaticObjectMethod(bitmapCls,
                                                    createBitmapFunction,
                                                    width,
                                                    height,
                                                    bitmapConfig);

    //
    // putting the pixels into the new bitmap:
    //
    if ((ret = AndroidBitmap_lockPixels(env, newBitmap, &bitmapPixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels()2 failed ! error=%d", ret);
        return nullptr;
    }

    uint32_t *newBitmapPixels = (uint32_t *) bitmapPixels;

    switch (operation) {
        case 0:
            rotate(newBitmapPixels, tempPixels, &info);
            break;
        case 1:
            filter(newBitmapPixels, tempPixels, &info);
            break;
    }

    AndroidBitmap_unlockPixels(env, newBitmap);

    //
    // freeing the native memory used to store the pixels
    //
    delete[] tempPixels;
    return newBitmap;
}

