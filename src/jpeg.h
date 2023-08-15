#ifndef JPEG_H
#define JPEG_H

#include "hufftree.h"
#include "image.h"

typedef struct {
    struct jpeg_component_info {
        unsigned char HSamplingFactor;
        unsigned char VSamplingFactor;
        unsigned char quantizationTableID;
    } componentInfo[256];
    unsigned char  quantizationTables[4][64]; // Quantization tables used in decoding
    huffnode       huffmanTables[2][4];       // Huffman tables used in decoding
    unsigned short width;                     // Width of the image (max is 65535)
    unsigned short height;                    // Height of the image (max is 65535)
    unsigned char  precision;
} jpeg_info;

typedef struct {
    struct scan_component_info {
        unsigned char componentID;
        unsigned char DCHuffmanTableID;
        unsigned char ACHuffmanTableID;
    } componentInfo[4];
    unsigned char maxHSamplingFactor;
    unsigned char maxVSamplingFactor;
    unsigned char componentCount;
} scan_info;

int  jpeg_read_info(jpeg_info *jpegInfo, scan_info *scanInfo, FILE *fp);
void jpeg_read_image(pixel *img, const jpeg_info *info, const scan_info *scanInfo, FILE *fp);

void jpeg_free(jpeg_info *info);

#endif
