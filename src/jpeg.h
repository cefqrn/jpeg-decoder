#ifndef JPEG_H
#define JPEG_H

#include "hufftree.h"
#include "image.h"

#include <stdint.h>

typedef struct component_data {
    uint8_t id;
    uint8_t qTableId;
    uint8_t hTreeId;
    uint8_t vSamplingFactor:4;
    uint8_t hSamplingFactor:4;
} component_data;

typedef struct {
    uint16_t quantTables[2][64];      // Quantization tables used to decode the image
    huffnode huffTrees[2][2];         // Huffman trees used to decode the image
    component_data componentData[3];
    uint16_t width;                   // Width of the image
    uint16_t height;                  // Height of the image
    uint16_t hPixelDensity;
    uint16_t vPixelDensity;
    uint8_t versionMajor;
    uint8_t versionMinor;
    uint8_t pixelDensityUnit;         // Unit for the horizontal and vertical pixel densities
    uint8_t precision;
    uint8_t componentCount;           // Determines whether the image has chrominance data. Either 1 or 3.
} jpeg_info;

void jpeg_read_info(jpeg_info *info, FILE *fp);
void jpeg_read_image(pixel *img, jpeg_info *info, FILE *fp);

void jpeg_free(jpeg_info *info);

#endif
