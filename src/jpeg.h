#ifndef JPEG_H
#define JPEG_H

#include "image.h"
#include <stdint.h>

image *jpg_fparse(char *path);
void jpg_free_image(image *im);

void jpg_yuv_to_rgb(image *im);
uint8_t ***jpg_get_image_pixels(image *im);

#endif