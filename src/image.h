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

void img_print_image(image *im, size_t pixelWidth, size_t maxWidth, size_t maxHeight);
image *img_create_image(size_t width, size_t height);
void img_yuv_to_rgb(image *im);
void img_rgb_to_yuv(image *im);
void img_free_image(image *im);

#endif