#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

typedef struct image image;

void img_print_image(image *im, size_t pixelWidth, size_t maxWidth, size_t maxHeight);
image *img_create_image(size_t width, size_t height);
void img_set_pixel(image *im, size_t x, size_t y, size_t componentIndex, uint8_t componentValue);
void img_yuv_to_rgb(image *im);
void img_rgb_to_yuv(image *im);
void img_free_image(image *im);
size_t img_get_width(image *im);
size_t img_get_height(image *im);

#endif