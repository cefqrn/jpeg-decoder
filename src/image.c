#include "image.h"

#include <stddef.h>
#include <stdio.h>

/*
    Print an RGB image.

    Prints rows up to the lowest between imageHeight and maxPrintHeight.
    Prevents lines from going longer than maxPrintWidth.

    Displays colors with spaces and ANSI 24-bit color escape codes.
*/
void image_print(pixel *img, size_t imageWidth, size_t imageHeight, size_t maxPrintWidth, size_t maxPrintHeight, size_t pixelWidth) {
    maxPrintHeight = maxPrintHeight < imageHeight ? maxPrintHeight : imageHeight;

    for (size_t y=0; y < maxPrintHeight; ++y) {
        for (size_t x=0; x < imageWidth && x * pixelWidth < maxPrintWidth; ++x) {
            char color[64];

            pixel p = img[y * imageWidth + x];
            snprintf(color, 64, "\x1b[48;2;%u;%u;%um", p.RGB.R, p.RGB.G, p.RGB.B);

            printf("%s", color);
            for (size_t i=0; i < pixelWidth; ++i) {
                putchar(' ');
            }
        }
        
        printf("\x1b[0m\n");
    }
}

// Clamp a value between 0 and 255 (inclusive).
static inline double clamp(double n) {
    return n < 0 ? 0 : 255 < n ? 255 : n;
}

// Convert an image in place from YCbCr to RGB.
void image_ycbcr_to_rgb(pixel *img, size_t width, size_t height) {
    for (size_t y=0; y < height; ++y) {
        for (size_t x=0; x < width; ++x) {
            pixel prev = img[y * width + x];

            img[y * width + x] = (pixel){
                .RGB.R = clamp(prev.YCbCr.Y                                     +   1.402 * (prev.YCbCr.Cr - 128.0)),
                .RGB.G = clamp(prev.YCbCr.Y - 0.34414 * (prev.YCbCr.Cb - 128.0) - 0.71414 * (prev.YCbCr.Cr - 128.0)),
                .RGB.B = clamp(prev.YCbCr.Y +   1.772 * (prev.YCbCr.Cb - 128.0))
            };
        }
    }
}

// Convert an image in place from RGB to YCbCr.
void image_rgb_to_ycbcr(pixel *img, size_t width, size_t height) {
    for (size_t y=0; y < height; ++y) {
        for (size_t x=0; x < width; ++x) {
            pixel prev = img[y * width + x];

            img[y * width + x] = (pixel){
                .YCbCr.Y  = clamp(  0.299 * prev.RGB.R +  0.587 * prev.RGB.G +  0.114 * prev.RGB.B),
                .YCbCr.Cb = clamp(-0.1687 * prev.RGB.R - 0.3313 * prev.RGB.G +    0.5 * prev.RGB.B + 128.0),
                .YCbCr.Cr = clamp(    0.5 * prev.RGB.R - 0.4187 * prev.RGB.G - 0.0813 * prev.RGB.B + 128.0)
            };
        }
    }
}
