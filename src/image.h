#ifndef IMAGE_H
#define IMAGE_H

#include <stddef.h>
#include <stdint.h>

typedef union pixel {
    uint8_t data[3];
    struct {
        uint8_t R;
        uint8_t G;
        uint8_t B;
    } RGB;
    struct {
        uint8_t Y;
        uint8_t U;
        uint8_t V;
    } YUV;
    struct {
        uint8_t Y;
        uint8_t Cb;
        uint8_t Cr;
    } YCbCr;
} pixel;

typedef pixel *image;

typedef struct image_info {
    size_t width;
    size_t height;
} image_info;

void image_print(image im, size_t pixelWidth, size_t maxWidth, size_t maxHeight);

image image_create(size_t width, size_t height);
void image_destroy(image im);

image_info image_info_of(image im);

void image_yuv_to_rgb(image im);
void image_rgb_to_yuv(image im);

#endif
