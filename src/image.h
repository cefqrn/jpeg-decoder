#ifndef IMAGE_H
#define IMAGE_H

#include <stddef.h>

typedef union {
    struct {
        unsigned char Y;
        unsigned char U;
        unsigned char V;
    } YUV;
    struct {
        unsigned char R;
        unsigned char G;
        unsigned char B;
    } RGB;
    unsigned char data[3];
} pixel;

#define image_size(width, height) (width * height * sizeof(pixel))

void image_print(pixel *img, size_t imageWidth, size_t imageHeight, size_t maxPrintWidth, size_t maxPrintHeight, size_t pixelWidth);

void image_yuv_to_rgb(pixel *img, size_t width, size_t height);
void image_rgb_to_yuv(pixel *img, size_t width, size_t height);

#endif
