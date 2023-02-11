#ifndef IMAGE_H
#define IMAGE_H

typedef union {
    struct {
        unsigned char Y, Cb, Cr;
    } YCbCr;
    struct {
        unsigned char R, G, B;
    } RGB;
    unsigned char data[3];
} pixel;

#define image_size(width, height) ((width) * (height) * sizeof(pixel))

void image_print(const pixel *img, unsigned imageWidth, unsigned imageHeight, unsigned maxPrintWidth, unsigned maxPrintHeight, unsigned pixelWidth);

void image_ycbcr_to_rgb(pixel *img, unsigned width, unsigned height);
void image_rgb_to_ycbcr(pixel *img, unsigned width, unsigned height);

#endif
