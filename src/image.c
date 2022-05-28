#include "macros.h"
#include "image.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct image {
    size_t width;
    size_t height;
    uint8_t (**pixels)[3];
} image;

void img_print_image(image *im, size_t pixelWidth, size_t maxWidth, size_t maxHeight) {
    for (size_t y=0; y < im->height && y < maxHeight; ++y) {
        for (size_t x=0; x < im->width && x * pixelWidth < maxWidth; ++x) {
            char color[64];
            snprintf(color, 64, "\x1b[48;2;%u;%u;%um", im->pixels[x][y][0], im->pixels[x][y][1], im->pixels[x][y][2]);
            for (size_t i=0; i < pixelWidth; ++i) {
                printf("%s ", color);
            }
        }
        
        printf("\x1b[0m\n");
    }
}

image *img_create_image(size_t width, size_t height) {
    image *im = malloc(sizeof *im);
    CHECK_ALLOC(im, "image");

    im->width = width;
    im->height = height;

    im->pixels = malloc(im->width * sizeof *im->pixels);
    CHECK_ALLOC(im->pixels, "pixels in image");
    
    for (size_t i=0; i < im->width; ++i) {
        im->pixels[i] = calloc(im->height, sizeof **im->pixels);
        CHECK_ALLOC(im->pixels[i], "pixels in image");
    }

    return im;
}

void img_set_pixel(image *im, size_t x, size_t y, size_t componentIndex, uint8_t componentValue) {
    im->pixels[x][y][componentIndex] = componentValue;
}

static inline uint8_t clamp(double n) {
    return n < 0 ? 0 : 255 < n ? 255 : n;
}

void img_yuv_to_rgb(image *im) {
    for (size_t x=0; x < im->width; ++x) {
        for (size_t y=0; y < im->height; ++y) {
            uint8_t pixel[3];
            memcpy(pixel, im->pixels[x][y], 3 * sizeof *pixel);

            im->pixels[x][y][0] = clamp(pixel[0]                                +   1.402 * (pixel[2] - 128.0));
            im->pixels[x][y][1] = clamp(pixel[0] - 0.34414 * (pixel[1] - 128.0) - 0.71414 * (pixel[2] - 128.0));
            im->pixels[x][y][2] = clamp(pixel[0] +   1.772 * (pixel[1] - 128.0));
        }
    }
}

void img_rgb_to_yuv(image *im) {
    for (size_t x=0; x < im->width; ++x) {
        for (size_t y=0; y < im->height; ++y) {
            uint8_t pixel[3];
            memcpy(pixel, im->pixels[x][y], 3 * sizeof *pixel);

            im->pixels[x][y][0] = clamp(  0.299 * pixel[0] +  0.587 * pixel[1] +  0.114 * pixel[2]);
            im->pixels[x][y][1] = clamp(-0.1687 * pixel[0] - 0.3313 * pixel[1] +    0.5 * pixel[2] + 128.0);
            im->pixels[x][y][2] = clamp(    0.5 * pixel[0] - 0.4187 * pixel[1] - 0.0813 * pixel[2] + 128.0);
        }
    }
}

void img_free_image(image *im) {
    FREE_2D_ARRAY(im->pixels, im->width);
    free(im);
}

size_t img_get_width(image *im) {
    return im->width;
}

size_t img_get_height(image *im) {
    return im->height;
}