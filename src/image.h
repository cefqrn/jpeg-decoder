#ifndef IMAGE_H
#define IMAGE_H

#include <stddef.h>
#include <stdint.h>

typedef union {
    struct {
        uint8_t Y;
        uint8_t U;
        uint8_t V;
    } YUV;
    struct {
        uint8_t R;
        uint8_t G;
        uint8_t B;
    } RGB;
    uint8_t data[3];
} pixel;

#define image_size(width, height) (width * height * sizeof(pixel))

void image_print(pixel *img, size_t imageWidth, size_t imageHeight, size_t maxPrintWidth, size_t maxPrintHeight, size_t pixelWidth);

void image_yuv_to_rgb(pixel *img, size_t width, size_t height);
void image_rgb_to_yuv(pixel *img, size_t width, size_t height);

#endif
