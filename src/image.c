#include "macros.h"
#include "image.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct image {
    uint16_t width;
    uint16_t height;
    uint8_t (**pixels)[3];
} image;

void img_print_image(image *im) {
    char pixels[] = " .:!=?$#@";
    for (size_t y=0; y < im->height; ++y) {
        for (size_t x=0; x < im->width; ++x) {
            char pixelValue = pixels[(int)((double)im->pixels[x][y][0]/255.0*8.0)];
            printf("%c%c", pixelValue, pixelValue);
        }
        
        printf("\n");
    }
}

image *img_create_image(uint16_t width, uint16_t height) {
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

void img_set_pixel(image *im, uint16_t x, uint16_t y, uint8_t componentIndex, uint8_t componentValue) {
    im->pixels[x][y][componentIndex] = componentValue;
}

void img_yuv_to_rgb(image *im) {
    uint8_t (**yuvImage)[3] = malloc(im->width * sizeof *yuvImage);
    CHECK_ALLOC(yuvImage, "image");

    for (size_t x=0; x < im->width; ++x) {
        yuvImage[x] = malloc(im->height * sizeof **yuvImage);
        CHECK_ALLOC(yuvImage[x], "image");

        for (size_t y=0; y < im->height; ++y) {
            for (size_t i=0; i < 3; ++i) {
                yuvImage[x][y][i] = im->pixels[x][y][i];
            }
        }
    }
    
    for (size_t x=0; x < im->width; ++x) {
        for (size_t y=0; y < im->height; ++y) {
            // Y
            im->pixels[x][y][0] = yuvImage[x][y][0];
            im->pixels[x][y][1] = yuvImage[x][y][0];
            im->pixels[x][y][2] = yuvImage[x][y][0];

            // Cb
            im->pixels[x][y][1] -= 0.34414 * (yuvImage[x][y][1] - 128);
            im->pixels[x][y][2] += 1.772 * (yuvImage[x][y][1] - 128);

            // Cr
            im->pixels[x][y][0] -= 1.402 * (yuvImage[x][y][2] - 128);
            im->pixels[x][y][1] += 0.71414 * (yuvImage[x][y][2] - 128);
        }
    }

    FREE_2D_ARRAY(yuvImage, im->width);
}

void img_free_image(image *im) {
    FREE_2D_ARRAY(im->pixels, im->width);
    free(im);
}