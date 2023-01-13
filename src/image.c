#include "macros.h"
#include "image.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void image_print(image im, size_t pixelWidth, size_t maxWidth, size_t maxHeight) {
    image_info info = image_info_of(im);

    for (size_t y=0; y < info.height && y < maxHeight; ++y) {
        for (size_t x=0; x < info.width && x * pixelWidth < maxWidth; ++x) {
            char color[64];

            pixel p = im[y * info.width + x];
            snprintf(color, 64, "\x1b[48;2;%u;%u;%um", p.RGB.R, p.RGB.G, p.RGB.B);

            printf("%s", color);
            for (size_t i=0; i < pixelWidth; ++i) {
                putchar(' ');
            }
        }
        
        printf("\x1b[0m\n");
    }
}

image image_create(size_t width, size_t height) {
    image im = calloc(1, sizeof(image_info) + width * height * sizeof(pixel));
    CHECK_ALLOC(im, "image");

    image_info *info = (image_info *)im;

    info->width = width;
    info->height = height;

    return (image)(info + 1);
}

void image_destroy(image im) {
    free((image_info *)im - 1);
}

image_info image_info_of(image im) {
    return *((image_info *)im - 1);
}

static inline uint8_t clamp(double n) {
    return n < 0 ? 0 : 255 < n ? 255 : n;
}

void image_yuv_to_rgb(image im) {
    image_info info = image_info_of(im);

    for (size_t y=0; y < info.height; ++y) {
        for (size_t x=0; x < info.width; ++x) {
            pixel prev = im[y * info.width + x];

            im[y * info.width + x] = (pixel){
                .RGB.R = clamp(prev.YUV.Y                                  +   1.402 * (prev.YUV.V - 128.0)),
                .RGB.G = clamp(prev.YUV.Y - 0.34414 * (prev.YUV.U - 128.0) - 0.71414 * (prev.YUV.V - 128.0)),
                .RGB.B = clamp(prev.YUV.Y +   1.772 * (prev.YUV.U - 128.0))
            };
        }
    }
}

void image_rgb_to_yuv(image im) {
    image_info info = image_info_of(im);

    for (size_t y=0; y < info.height; ++y) {
        for (size_t x=0; x < info.width; ++x) {
            pixel prev = im[y * info.width + x];

            im[y * info.width + x] = (pixel){
                .YUV.Y = clamp(  0.299 * prev.RGB.R +  0.587 * prev.RGB.G +  0.114 * prev.RGB.B),
                .YUV.U = clamp(-0.1687 * prev.RGB.R - 0.3313 * prev.RGB.G +    0.5 * prev.RGB.B + 128.0),
                .YUV.V = clamp(    0.5 * prev.RGB.R - 0.4187 * prev.RGB.G - 0.0813 * prev.RGB.B + 128.0)
            };
        }
    }
}
