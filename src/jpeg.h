#ifndef JPEG_H
#define JPEG_H

#include <stdint.h>

typedef struct image {
    uint16_t width;
    uint16_t height;
    uint8_t (**pixels)[3]
} image;

image *jpg_fparse(char *path);
void free_image(image *im);

#endif