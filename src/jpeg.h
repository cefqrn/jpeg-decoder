#ifndef JPEG_H
#define JPEG_H

#include <stdint.h>

typedef struct image {
    uint16_t width;
    uint16_t height;
    uint8_t (**pixels)[3];
} image;

image *jpg_fparse(char *path);
void jpg_free_image(image *im);

uint8_t ***jpg_get_image_pixels(image *im);

#endif