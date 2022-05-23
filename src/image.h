#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

typedef struct image image;

void img_print_image(image *im, size_t pixelWidth, size_t component);
image *img_create_image(uint16_t width, uint16_t height);
void img_set_pixel(image *im, uint16_t x, uint16_t y, uint8_t componentIndex, uint8_t componentValue);
void img_yuv_to_rgb(image *im);
void img_free_image(image *im);

#endif