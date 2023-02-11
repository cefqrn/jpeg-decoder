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

static inline int decode_mcu_value(unsigned size, bitstream *str) {
    unsigned short bits = bitstream_get_bits(str, size);

    return !(bits >> (size - 1)) ? bits + 1 - (1 << size) : bits;
}

static int parse_coeff_matrix(int coeffMatrix[8][8], const jpeg_info *info, component_data componentData, bitstream *str, int *dcCoeff) {
    int coeffVector[64] = {0};

    const unsigned char *quantTable = info->quantTables[componentData.qTableId];

    {
        const huffnode *huffTreeDc = &info->huffTrees[componentData.hTreeId][CLASS_DC];

        unsigned size = hufftree_decode_next_symbol(huffTreeDc, str);
        *dcCoeff += decode_mcu_value(size, str);

        coeffVector[0] = *dcCoeff * quantTable[0];
    }

    const huffnode *huffTreeAc = &info->huffTrees[componentData.hTreeId][CLASS_AC];
    for (unsigned i=1; i < 64; ++i) {
        unsigned short value = hufftree_decode_next_symbol(huffTreeAc, str);

        if (value == 0x00)
            break;

        i += value >> 4;  // Skip zeroes

        if (i >= 64)
            return -1;

        unsigned size = value & 0xF;
        coeffVector[i] = decode_mcu_value(size, str) * quantTable[i];
    }

    for (unsigned x=0; x < 8; ++x) {
        for (unsigned y=0; y < 8; ++y) {
            coeffMatrix[x][y] = coeffVector[ZIGZAG[x][y]];
        }
    }

    return 0;
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

int decode_MCU(pixel *im, const jpeg_info *info, bitstream *str, component_data componentData, int *dcCoeff, unsigned McuX, unsigned McuY, unsigned hSamplingFactor, unsigned vSamplingFactor) {
    int coeffMatrix[8][8];
    if (parse_coeff_matrix(coeffMatrix, info, componentData, str, dcCoeff))
        return -1;

    unsigned short imageWidth = info->width;
    unsigned short imageHeight = info->height;

    for (unsigned x=0; x < 8 && McuX + x * hSamplingFactor < imageWidth; ++x) {
        for (unsigned y=0; y < 8 && McuY + y * vSamplingFactor < imageHeight; ++y) {
            unsigned char value = clamp(lround(idct(coeffMatrix, x, y)) + 128, 0, 255);

            unsigned short globalX = McuX + x * hSamplingFactor;
            unsigned short globalY = McuY + y * vSamplingFactor;

            for (unsigned h=0; h < hSamplingFactor && globalX + h < imageWidth; ++h) {
                for (unsigned v=0; v < vSamplingFactor && globalY + v < imageHeight; ++v) {
                    // Component id is one above our component index.
                    im[(globalY + v) * imageWidth + (globalX + h)].data[componentData.id - 1] = value;
                }
            }
        }
    }

    return 0;
}
