#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-magic-numbers"
#pragma ide diagnostic ignored "readability-magic-numbers"
//
// Created by Petr Kubes on 2019-09-08.
//

#include <ios>
#include "operations.h"

#define IDX(x, y, columns) (y * columns + x)


void rotate(uint32_t *newBitmapPixels, uint32_t *oldBitmapPixels, AndroidBitmapInfo *info) {
    int whereToPut = 0;

    for (int x = info->width - 1; x >= 0; --x) {
        for (int y = 0; y < info->height; ++y) {
            uint32_t pixel = oldBitmapPixels[info->width * y + x];
            newBitmapPixels[whereToPut++] = pixel;
        }
    }
}

u_int32_t getRed(u_int32_t pixel) {
    return pixel >> 16U & 0x0000ff;
}

u_int32_t getGreen(u_int32_t pixel) {
    return pixel >> 8U & 0x0000ff;
}

u_int32_t getBlue(u_int32_t pixel) {
    return pixel & 0x0000ff;
}

u_int32_t getA(u_int32_t pixel) {
    return pixel >> 24U & 0x0000ff;
}

uint32_t apply_mask(uint32_t pixels[9]) {
    uint32_t pixel;

    int r = 0;
    int g = 0;
    int b = 0;
    int a = 0;

    int mask[9] = {0, -1, 0, -1, 7, -1, 0, -1, 0};

    for (int i = 0; i < 9; i++) {
        r += mask[i] * (int)getRed(pixels[i]);
        g += mask[i] * (int)getGreen(pixels[i]);
        b += mask[i] * (int)getBlue(pixels[i]);
        //a += mask[i] * getA(pixels[i]);
    }

    r = ((r < 255) ? r : 255);
    r = ((r > 0) ? r : 0);
    g = ((g < 255) ? g : 255);
    g = ((g > 0) ? g : 0);
    b = ((b < 255) ? b : 255);
    b = ((b > 0) ? b : 0);
    a = ((a < 255) ? a : 255);
    a = ((a > 0) ? a : 0);

    //a = 255;

    uint32_t r32 = (uint32_t)r;
    uint32_t g32 = (uint32_t)g;
    uint32_t b32 = (uint32_t)b;

    pixel = 255U << 24U | r32 << 16U | g32 << 8U | b32;

    return pixel;
}

void filter(uint32_t *newBitmapPixels, uint32_t *oldBitmapPixels, AndroidBitmapInfo *info) {
    for (int y = 0; y < info->height; y++) {
        for (int x = 0; x < info->width; x++) {
            if (y == 0 || x == 0 || y == info->height - 1 || x == info->width - 1) {
                newBitmapPixels[IDX(x, y, info->width)] = oldBitmapPixels[IDX(x, y, info->width)];
            } else {
                uint32_t pixels[9];

                pixels[0] = oldBitmapPixels[IDX((x - 1), (y - 1), info->width)];
                pixels[1] = oldBitmapPixels[IDX((x), (y - 1), info->width)];
                pixels[2] = oldBitmapPixels[IDX((x + 1), (y - 1), info->width)];
                pixels[3] = oldBitmapPixels[IDX((x - 1), (y), info->width)];
                pixels[4] = oldBitmapPixels[IDX((x), (y), info->width)];
                pixels[5] = oldBitmapPixels[IDX((x + 1), (y), info->width)];
                pixels[6] = oldBitmapPixels[IDX((x - 1), (y + 1), info->width)];
                pixels[7] = oldBitmapPixels[IDX((x), (y + 1), info->width)];
                pixels[8] = oldBitmapPixels[IDX((x + 1), (y + 1), info->width)];

                newBitmapPixels[IDX(x, y, info->width)] = apply_mask(pixels);
            }
        }
    }
}


#pragma clang diagnostic pop