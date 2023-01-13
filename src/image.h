#ifndef IMAGE_H
#define IMAGE_H

#include <stddef.h>
#include <stdint.h>

typedef uint8_t (**pixelArray)[3];

typedef struct image {
    size_t width;
    size_t height;
    pixelArray pixels;
} image;

void image_print(image *im, size_t pixelWidth, size_t maxWidth, size_t maxHeight);

image *image_create(size_t width, size_t height);
void image_destroy(image *im);

void image_yuv_to_rgb(image *im);
void image_rgb_to_yuv(image *im);

#endif
