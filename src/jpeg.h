#ifndef JPEG_H
#define JPEG_H

#include "hufftree.h"
#include "image.h"

typedef struct component_data {
    unsigned char id;
    unsigned char qTableId;
    unsigned char hTreeId;
    unsigned char vSamplingFactor;
    unsigned char hSamplingFactor;
} component_data;

typedef struct {
    unsigned char  quantTables[2][64];  // Quantization tables used in decoding
    huffnode       huffTrees[2][2];     // Huffman trees used in decoding
    component_data componentData[3];
    unsigned short width;               // Width of the image (max is 65535)
    unsigned short height;              // Height of the image (max is 65535)
    unsigned short hPixelDensity;       // Density used in printing
    unsigned short vPixelDensity;       // Density used in printing
    unsigned char versionMajor;
    unsigned char versionMinor;
    unsigned char pixelDensityUnit;     // Unit for the horizontal and vertical pixel densities
    unsigned char precision;
    unsigned char componentCount;       // Determines whether the image has chrominance data. Either 1 or 3.
} jpeg_info;

int jpeg_read_info(jpeg_info *info, FILE *fp);
int jpeg_read_image(pixel *img, jpeg_info *info, FILE *fp);

void jpeg_free(jpeg_info *info);

#endif
