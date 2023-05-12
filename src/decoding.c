#include "bitstream.h"
#include "decoding.h"
#include "hufftree.h"
#include "image.h"
#include "jpeg.h"

#include <math.h>

static const unsigned ZIGZAG[8][8] = {
    { 0,  1,  5,  6, 14, 15, 27, 28},
    { 2,  4,  7, 13, 16, 26, 29, 42},
    { 3,  8, 12, 17, 25, 30, 41, 43},
    { 9, 11, 18, 24, 31, 40, 44, 53},
    {10, 19, 23, 32, 39, 45, 52, 54},
    {20, 22, 33, 38, 46, 51, 55, 60},
    {21, 34, 37, 47, 50, 56, 59, 61},
    {35, 36, 48, 49, 57, 58, 62, 63}
};

static const double IDCT_TABLE[8][8] = {
    {0.707107,  0.707107,  0.707107,  0.707107,  0.707107,  0.707107,  0.707107,  0.707107},
    {0.980785,  0.831470,  0.555570,  0.195090, -0.195090, -0.555570, -0.831470, -0.980785},
    {0.923880,  0.382683, -0.382683, -0.923880, -0.923880, -0.382683,  0.382683,  0.923880},
    {0.831470, -0.195090, -0.980785, -0.555570,  0.555570,  0.980785,  0.195090, -0.831470},
    {0.707107, -0.707107, -0.707107,  0.707107,  0.707107, -0.707107, -0.707107,  0.707107},
    {0.555570, -0.980785,  0.195090,  0.831470, -0.831470, -0.195090,  0.980785, -0.555570},
    {0.382683, -0.923880,  0.923880, -0.382683, -0.382683,  0.923880, -0.923880,  0.382683},
    {0.195090, -0.555570,  0.831470, -0.980785,  0.980785, -0.831470,  0.555570, -0.195090}
};

static inline int decode_coefficient(unsigned size, unsigned bits) {
    return bits >> (size - 1) ? bits : (long)bits - (1 << size) + 1;
}

static void decode_coefficient_matrix(int coeffMatrix[8][8], const jpeg_info *jpegInfo, struct scan_component_info componentInfo, bitstream *str, int *dcCoeff) {
    int coeffVector[64] = {0};

    const unsigned char *quantTable = jpegInfo->quantizationTables[jpegInfo->componentInfo[componentInfo.componentID].quantizationTableID];

    {
        const huffnode *huffTreeDc = &jpegInfo->huffmanTables[CLASS_DC][componentInfo.DCHuffmanTableID];

        unsigned size = hufftree_decode_next_symbol(huffTreeDc, str);
        *dcCoeff += decode_coefficient(size, bitstream_get_bits(str, size));

        coeffVector[0] = *dcCoeff * quantTable[0];
    }

    const huffnode *huffTreeAc = &jpegInfo->huffmanTables[CLASS_AC][componentInfo.ACHuffmanTableID];
    for (unsigned i=1; i < 64; ++i) {
        unsigned short value = hufftree_decode_next_symbol(huffTreeAc, str);

        unsigned skipped_zeros = value >> 4;
        unsigned coefficient_size = value & 0x0f;

        i += skipped_zeros;
        if (coefficient_size == 0) {
            if (skipped_zeros == 0xf) {
                continue;
            } else {
                break;
            }
        }

        coeffVector[i] = decode_coefficient(coefficient_size, bitstream_get_bits(str, coefficient_size)) * quantTable[i];
    }

    for (unsigned x=0; x < 8; ++x) {
        for (unsigned y=0; y < 8; ++y) {
            coeffMatrix[x][y] = coeffVector[ZIGZAG[x][y]];
        }
    }
}

static double idct(const int coeffMatrix[8][8], unsigned x, unsigned y) {
    double sum = 0;
    for (unsigned u=0; u < 8; ++u) {
        for (unsigned v=0; v < 8; ++v) { 
            sum += coeffMatrix[v][u] * IDCT_TABLE[u][x] * IDCT_TABLE[v][y];
        }
    }

    return sum / 4;
}

static inline int clamp(int n, int min, int max) {
    return n < min ? min : n > max ? max : n;
}

void decode_data_unit(pixel *im, const jpeg_info *info, struct scan_component_info componentInfo, bitstream *str, int *dcCoeff, unsigned globalX, unsigned globalY, unsigned HStretchFactor, unsigned VStretchFactor) {
    int coeffMatrix[8][8];
    decode_coefficient_matrix(coeffMatrix, info, componentInfo, str, dcCoeff);

    unsigned short imageWidth = info->width;
    unsigned short imageHeight = info->height;

    for (unsigned x=0; x < 8 && (unsigned long)globalX + x * HStretchFactor < imageWidth; ++x) {
        for (unsigned y=0; y < 8 && (unsigned long)globalY + y * VStretchFactor < imageHeight; ++y) {
            unsigned char value = clamp(lround(idct(coeffMatrix, x, y)) + 128, 0, 255);

            unsigned short pixelX = globalX + x * HStretchFactor;
            unsigned short pixelY = globalY + y * VStretchFactor;

            for (unsigned h=0; h < HStretchFactor && (unsigned long)globalX + h < imageWidth; ++h) {
                for (unsigned v=0; v < VStretchFactor && (unsigned long)globalY + v < imageHeight; ++v) {
                    // Component id is one above our component index.
                    im[(pixelY + v) * imageWidth + (pixelX + h)].data[componentInfo.componentID - 1] = value;
                }
            }
        }
    }
}
