//
// Created by Petr Kubes on 2019-09-08.
//

#ifndef CONVOLUTIONCAMERA_OPERATIONS_H
#define CONVOLUTIONCAMERA_OPERATIONS_H

#include <android/log.h>
#include <android/bitmap.h>

#define  LOG_TAG    "DEBUG"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


void rotate(uint32_t *newBitmapPixels, uint32_t *oldBitmapPixels, AndroidBitmapInfo *info);
void filter(uint32_t *newBitmapPixels, uint32_t *oldBitmapPixels, AndroidBitmapInfo *info);

#endif //CONVOLUTIONCAMERA_OPERATIONS_H
