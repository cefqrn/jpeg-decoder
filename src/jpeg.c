#include "bitstream.h"
#include "hufftree.h"
#include "macros.h"
#include "image.h"
#include "jpeg.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define CHAR_WIDTH  8
#define MARKER_SIZE 2

#define READ_WORD(fp) getc(fp) << CHAR_WIDTH | getc(fp)
#define GET_WORD(data, index) (data[index] << CHAR_WIDTH) + data[index + 1]

static const size_t ZIGZAG[8][8] = {
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

typedef enum SegmentMarker {
    SOI  = 0xFFD8,  // Start of Image
    COM  = 0xFFFE,  // Comment
    APP0 = 0xFFE0,  // Application Marker (JFIF)
    SOF0 = 0xFFC0,  // Start of Frame (Baseline)
    DHT  = 0xFFC4,  // Define Huffman Table
    DQT  = 0xFFDB,  // Define Quantization Table
    SOS  = 0xFFDA,  // Start of Scan
} SegmentMarker;

typedef enum ComponentId {
    ID_Y  = 1,
    ID_CB = 2,
    ID_CR = 3
} ComponentId;

static void parse_APP0(jpeg_info *info, uint8_t *data) {
    char identifier[5];
    strncpy(identifier, (char *)data, 5);

    if (strcmp(identifier, "JFIF") == 0) {
        CHECK_FAIL((info->versionMajor = data[5]) != 1, "Invalid major version number: %d", info->versionMajor);

        info->versionMinor = data[6];
        info->pixelDensityUnit = data[7];
        info->hPixelDensity = GET_WORD(data, 8);
        info->vPixelDensity = GET_WORD(data, 10);
    } else if (strcmp(identifier, "JFXX")) {
        // Thumbnails are ignored
        FAIL("Invalid APP0 identifier: %s", identifier);
    }
}

static void parse_SOF0(jpeg_info *info, uint8_t *data) {
    info->precision = data[0];
    info->height = GET_WORD(data, 1);
    info->width = GET_WORD(data, 3);
    info->componentCount = data[5];

    for (size_t i=0; i < info->componentCount; ++i) {
        info->componentData[i] = (component_data){
            .id = data[6 + i*3],
            .vSamplingFactor = data[7 + i*3] >> 4,
            .hSamplingFactor = data[7 + i*3] & 0xF,
            .qTableId = data[8 + i*3] & 0xF
        };
    }
}

static void parse_DHT(jpeg_info *info, uint8_t *data, size_t length) {
    size_t offset = 0;

    do {
        size_t class = (data[offset] & 0x10) >> 4;
        size_t id = data[offset] & 0x1;

        ++offset;

        offset += hufftree_parse(&info->huffTrees[id][class], data + offset);
    } while (offset < length);
}

static void parse_DQT(jpeg_info *info, uint8_t *data, size_t length) {
    size_t offset = 0;
    
    do {
        size_t id = data[offset++] & 0x1;
        for (size_t i=0; i < 64; ++i)
            info->quantTables[id][i] = data[offset++];
    } while (offset < length);
}

static void parse_SOS(jpeg_info *info, uint8_t *data) {
    uint8_t componentCount = data[0];
    for (size_t i=0; i < componentCount; ++i) {
        info->componentData[i].hTreeId = data[2 + i * 2] >> 4;
    }
}

void jpeg_read_info(jpeg_info *info, FILE *fp) {
    {  // Read the file until the start of the image is reached.
        uint16_t word = READ_WORD(fp);
        while (word != SOI && !feof(fp) && !ferror(fp)) {
            word = (word << CHAR_WIDTH) + getc(fp);
        }

        CHECK_FAIL(word != SOI, "Start of image never reached.");
    }

    SegmentMarker marker;
    do {
        marker = READ_WORD(fp);
        size_t length = READ_WORD(fp) - MARKER_SIZE;

        uint8_t *data = malloc(length * sizeof *data);
        CHECK_ALLOC(data, "data");

        CHECK_FAIL(fread(data, sizeof *data, length, fp) != length, "Couldn't read image segment.");

        switch (marker) {
            case APP0: parse_APP0(info, data);        break;
            case SOF0: parse_SOF0(info, data);        break;
            case DHT:  parse_DHT(info, data, length); break;
            case DQT:  parse_DQT(info, data, length); break;
            case SOS:  parse_SOS(info, data);         break;
            default:;  // All other markers are ignored.
        }

        free(data);
    } while (marker != SOS);
}

static inline int decode_MCU_value(uint8_t size, int16_t bits) {
    if (!(bits >> (size - 1))) {
        bits += 1 - (1 << size);
    }

    return bits;
}

static inline int decode_dc_diff(huffnode *tree, bitstream *str, int prevCoeff) {
    uint8_t size = hufftree_decode_next_symbol(tree, str);
    int32_t bits = bitstream_get_bits(str, size);

    return decode_MCU_value(size, bits) + prevCoeff;
}

static int parse_coeff_matrix(int coeffMatrix[8][8], jpeg_info *info, component_data componentData, bitstream *str, int prevDcCoeff) {
    int coeffVector[64] = {0};

    huffnode *huffTrees = info->huffTrees[componentData.hTreeId];
    uint16_t *quantTable = info->quantTables[componentData.qTableId];

    int dcCoeff = decode_dc_diff(huffTrees + CLASS_DC, str, prevDcCoeff);
    coeffVector[0] = dcCoeff * quantTable[0];

    for (size_t i=1; i < 64; ++i) {
        uint8_t value = hufftree_decode_next_symbol(huffTrees + CLASS_AC, str);

        if (!value)
            break;

        i += value >> 4;  // Skip zeroes
        uint8_t size = value & 0xF;
        
        CHECK_FAIL(64 <= i, "Coefficient value index went past 64.");

        coeffVector[i] = decode_MCU_value(size, bitstream_get_bits(str, size)) * quantTable[i];
    }

    for (size_t x=0; x < 8; ++x) {
        for (size_t y=0; y < 8; ++y) {
            coeffMatrix[x][y] = coeffVector[ZIGZAG[x][y]];
        }
    }

    return dcCoeff;
}

static float idct(int coeffMatrix[8][8], size_t x, size_t y) {
    float sum = 0;
    for (size_t u=0; u < 8; ++u) {
        for (size_t v=0; v < 8; ++v) { 
            sum += coeffMatrix[v][u] * IDCT_TABLE[u][x] * IDCT_TABLE[v][y];
        }
    }

    return sum / 4;
}

static inline int clamp(int n, int min, int max) {
    return n < min ? min : n > max ? max : n;
}

static int decode_MCU(jpeg_info *info, pixel *im, bitstream *str, component_data componentData, int prevDcCoeff, size_t McuX, size_t McuY, size_t hSamplingFactor, size_t vSamplingFactor) {
    int coeffMatrix[8][8];
    int dcCoeff = parse_coeff_matrix(coeffMatrix, info, componentData, str, prevDcCoeff);

    unsigned width = info->width;
    unsigned height = info->height;

    for (size_t x=0; x < 8 && McuX + x * hSamplingFactor < width; ++x) {
        for (size_t y=0; y < 8 && McuY + y * vSamplingFactor < height; ++y) {
            uint8_t value = clamp(lround(idct(coeffMatrix, x, y)) + 128, 0, 255);

            uint16_t globalX = McuX + x * hSamplingFactor;
            uint16_t globalY = McuY + y * vSamplingFactor;

            for (size_t h=0; h < hSamplingFactor && globalX + h < width; ++h) {
                for (size_t v=0; v < vSamplingFactor && globalY + v < height; ++v) {
                    // Component id is one above index in a YUV pixel.
                    im[(globalY + v) * width + (globalX + h)].data[componentData.id - 1] = value;
                }
            }
        }
    }

    return dcCoeff;
}

void jpeg_read_image(pixel *img, jpeg_info *info, FILE *fp) {
    bitstream stream = bitstream_create(fp);

    int dcCoeffs[3] = {0};

    // Note: this assumes both Cb and Cr have the same sampling factors.
    size_t hSamplingFactor = 1;
    size_t vSamplingFactor = 1;
    if (info->componentCount == 3) {
        hSamplingFactor = info->componentData[0].hSamplingFactor / info->componentData[1].hSamplingFactor;
        vSamplingFactor = info->componentData[0].vSamplingFactor / info->componentData[1].vSamplingFactor;
    }
    
    for (int y=0; y < info->height; y += 8 * vSamplingFactor) {
        for (int x=0; x < info->width; x += 8 * hSamplingFactor) {
            for (size_t v=0; v < vSamplingFactor; ++v) {
                for (size_t h=0; h < hSamplingFactor; ++h) {
                    dcCoeffs[0] = decode_MCU(info, img, &stream, info->componentData[0], dcCoeffs[0], x + 8*h, y + 8*v, 1, 1);
                }
            }

            if (info->componentCount == 3) {
                dcCoeffs[1] = decode_MCU(info, img, &stream, info->componentData[1], dcCoeffs[1], x, y, hSamplingFactor, vSamplingFactor);
                dcCoeffs[2] = decode_MCU(info, img, &stream, info->componentData[2], dcCoeffs[2], x, y, hSamplingFactor, vSamplingFactor);
            }
        }
    }
}

// Free the memory taken by info.
void jpeg_free(jpeg_info *info) {
    for (size_t i=0; i < 2; ++i) {
        for (size_t j=0; j < 2; ++j) {
            hufftree_destroy(&info->huffTrees[i][j]);
        }
    }
}
