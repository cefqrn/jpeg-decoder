#include "quanttable.h"
#include "hufftree.h"
#include "macros.h"
#include "stream.h"
#include "image.h"
#include "err.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define CHAR_WIDTH 8
#define WORD_SIZE 2
#define READ_WORD(fp) getc(fp) << CHAR_WIDTH | getc(fp)
#define GET_WORD(data, index) (data[index] << CHAR_WIDTH) + data[index + 1]

size_t ZIGZAG[8][8] = {
    {0, 1, 5, 6, 14, 15, 27, 28},
    {2, 4, 7, 13, 16, 26, 29, 42},
    {3, 8, 12, 17, 25, 30, 41, 43},
    {9, 11, 18, 24, 31, 40, 44, 53},
    {10, 19, 23, 32, 39, 45, 52, 54},
    {20, 22, 33, 38, 46, 51, 55, 60},
    {21, 34, 37, 47, 50, 56, 59, 61},
    {35, 36, 48, 49, 57, 58, 62, 63}
};

typedef enum SegmentMarker {
    SOI = 0xFFD8,
    COM = 0xFFFE,
    APP0 = 0xFFE0,
    SOF = 0xFFC0,
    DHT = 0xFFC4,
    DQT = 0xFFDB,
    SOS = 0xFFDA,
} SegmentMarker;

typedef enum ComponentId {
    ID_Y = 1,
    ID_CB = 2,
    ID_CR = 3,
    ID_I = 4,
    ID_Q = 5
} ComponentId;

typedef enum TableClass {
    CLASS_DC = 0,
    CLASS_AC = 1
} TableClass;

typedef struct component_data {
    uint8_t id;
    uint8_t vSamplingFactor:4;
    uint8_t hSamplingFactor:4;
    uint8_t qTableId;
    uint8_t hTreeId;
} component_data;

typedef struct jpeg_data {
    uint8_t versionMajor;
    uint8_t versionMinor;
    uint8_t pixelDensityUnit;
    uint8_t hPixelDensity;
    uint8_t vPixelDensity;
    uint16_t width;
    uint16_t height;
    uint8_t precision;
    uint8_t componentCount;
    component_data **componentData;
    huff_tree *huffTrees[2][2];
    quant_table *quantTables[2];
} jpeg_data;

static void parse_APP0(jpeg_data *imageData, uint8_t *data, size_t length) {
    if ((imageData->versionMajor = data[5]) != 1)
        errx(EXIT_FAILURE, "Invalid major version number: %d\n", imageData->versionMajor);

    imageData->versionMinor = data[6];
    imageData->pixelDensityUnit = data[7];
    imageData->hPixelDensity = GET_WORD(data, 8);
    imageData->vPixelDensity = GET_WORD(data, 10);
}

static void parse_SOF(jpeg_data *imageData, uint8_t *data, size_t length) {
    imageData->precision = data[0];
    imageData->height = GET_WORD(data, 1);
    imageData->width = GET_WORD(data, 3);
    imageData->componentCount = data[5];

    imageData->componentData = malloc(imageData->componentCount * sizeof *imageData->componentData);
    CHECK_ALLOC(imageData->componentData, "image component data");

    for (size_t i=0; i < imageData->componentCount; ++i) {
        component_data *currComponentData = malloc(sizeof *currComponentData);
        CHECK_ALLOC(currComponentData, "component data");

        currComponentData->id = data[6 + i*3];
        currComponentData->vSamplingFactor = data[7 + i*3] >> 4;
        currComponentData->hSamplingFactor = data[7 + i*3] & 0xF;
        currComponentData->qTableId = data[8 + i*3] & 0xF;

        imageData->componentData[i] = currComponentData;
    }
}

static void parse_DHT(jpeg_data *imageData, uint8_t *data, size_t length) {
    size_t offset = 0;
    while (offset < length) {
        huff_tree *tree = huf_parse_huff_tree(data, &offset);
        imageData->huffTrees[huf_get_huff_tree_id(tree)][huf_get_huff_tree_class(tree)] = tree;
    }
}

static void parse_DQT(jpeg_data *imageData, uint8_t *data, size_t length) {
    size_t offset = 0;
    while (offset < length) {
        quant_table *table = qnt_parse_quant_table(data, &offset);
        imageData->quantTables[qnt_get_quant_table_number(table)] = table;
    }
}

static void parse_SOS(jpeg_data *imageData, uint8_t *data, size_t length) {
    uint8_t componentCount = data[0];
    for (size_t i=0; i < componentCount; ++i) {
        imageData->componentData[i]->hTreeId = data[2 + i * 2] >> 4;
    }
}

static int decode_MCU_value(uint8_t size, int16_t bits) {
    if (!(bits >> (size - 1))) {
        bits += 1 - (1 << size);
    }

    return bits;
}

static int decode_dc_diff(huff_tree *tree, stream *str, int prevCoeff) {
    uint8_t size = huf_decode_next_symbol(tree, str);
    int32_t bits = str_get_bits(str, size);

    return decode_MCU_value(size, bits) + prevCoeff;
}

static double normalize(double u) {
    return u == 0 ? 1.0/sqrt(2.0) : 1.0;
}

static uint8_t clamp(int n) {
    return n < 0 ? 0 : n > 255 ? 255 : n;
}

static int decode_MCU(jpeg_data *imageData, image *im, stream *str, size_t componentIndex, int prevDcCoeff, size_t McuX, size_t McuY) {
    int coeffVector[64] = {0};

    component_data *componentData = imageData->componentData[componentIndex];

    huff_tree **huffTrees = imageData->huffTrees[componentData->hTreeId];
    quant_table *quantTable = imageData->quantTables[componentData->qTableId];

    prevDcCoeff = decode_dc_diff(huffTrees[CLASS_DC], str, prevDcCoeff);
    coeffVector[0] = prevDcCoeff * qnt_get_quant_table_value(quantTable, 0);

    for (size_t i=1; i < 64; ++i) {
        uint8_t value = huf_decode_next_symbol(huffTrees[CLASS_AC], str);

        if (!value)
            break;

        i += value >> 4; // skip zeroes
        uint8_t size = value & 0xF;
        
        if (64 <= i)
            errx(EXIT_FAILURE, "Coefficient value index went past 64.");

        coeffVector[i] = decode_MCU_value(size, str_get_bits(str, size)) * qnt_get_quant_table_value(quantTable, i);
    }

    int coeffMatrix[8][8];
    for (size_t x=0; x < 8; ++x) {
        for (size_t y=0; y < 8; ++y) {
            coeffMatrix[x][y] = coeffVector[ZIGZAG[x][y]];
        }
    }

    static double idctTable[8][8];
    if (idctTable[0][0] == 0) {
        for (size_t u=0; u < 8; ++u) {
            for (size_t x=0; x < 8; ++x) {
                idctTable[u][x] = normalize(u) * cos(((2.0*x + 1.0) * u * M_PI) / 16.0);
            }
        }
    }

    for (size_t x=0; x < 8 && McuX + x < imageData->width; ++x) {
        for (size_t y=0; y < 8 && McuY + y < imageData->height; ++y) {
            double sum = 0;
            for (size_t u=0; u < imageData->precision; ++u) {
                for (size_t v=0; v < imageData->precision; ++v) { 
                    sum += coeffMatrix[u][v] * idctTable[u][x] * idctTable[v][y];
                }
            }

            uint8_t value = clamp(round(sum/4 + 128));
            uint16_t globalX = McuX + x;
            uint16_t globalY = McuY + y;
            // printf("%d, %d: %d (%d)\n", globalX, globalY, value, componentData->id);

            img_set_pixel(im, globalX, globalY, componentData->id - 1, value);
        }
    }

    return prevDcCoeff;
}

static void parse_image_data(jpeg_data *imageData, image *im, uint8_t *data, size_t length) {
    // get rid of byte stuffing
    for (size_t i=0; i < length; ++i) {
        if (data[i] == 0xFF) {
            memcpy(data + i + 1, data + i + 2, length-- - i - 1);
        }
    }

    stream *str = str_create_stream(data, length);

    int dcCoeffs[5] = {0};
    
    for (int x=0; x < imageData->width; x += 8) {
        for (int y=0; y < imageData->height; y += 8) {
            for (size_t i=0; i < imageData->componentCount; ++i) {
                dcCoeffs[i] = decode_MCU(imageData, im, str, i, dcCoeffs[i], x, y);
            }
        }
    }
}

static void parse_segments(jpeg_data *imageData, FILE *fp) {
    SegmentMarker marker = 0;
    do {
        marker = READ_WORD(fp);

        size_t length = READ_WORD(fp) - WORD_SIZE;
        uint8_t *data = malloc(length * sizeof *data);
        CHECK_ALLOC(data, "data");

        fread(data, sizeof(*data), length, fp);

        switch (marker) {
            case COM: break;
            case APP0: parse_APP0(imageData, data, length); break;
            case SOF: parse_SOF(imageData, data, length); break;
            case DHT: parse_DHT(imageData, data, length); break;
            case DQT: parse_DQT(imageData, data, length); break;
            case SOS: parse_SOS(imageData, data, length); break;
            default: warnx("Could not recognize marker: %04X", marker);
        }

        free(data);
    } while (marker != SOS);
}

static void free_jpeg(jpeg_data *data) {
    for (int i=0; i < data->componentCount; ++i) {
        free(data->componentData[i]);
    }
    free(data->componentData);

    for (size_t i=0; i < 2; ++i) {
        for (size_t j=0; j < 2; ++j) {
            huf_free_huff_tree(data->huffTrees[i][j]);
        }
    }

    for (size_t i=0; i < 2; ++i) {
        qnt_free_quant_table(data->quantTables[i]);
    }

    free(data);
}

image *jpg_fparse(char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
        errx(EXIT_FAILURE, "Could not open file.");

    for (uint16_t word = READ_WORD(fp); word != SOI; word = (word << CHAR_WIDTH) + fgetc(fp)) {}

    jpeg_data *imageData = calloc(1, sizeof *imageData);
    CHECK_ALLOC(imageData, "image data");

    parse_segments(imageData, fp);

    image *im = img_create_image(imageData->width, imageData->height);

    {
        // get remaining file length
        size_t index = ftell(fp);
        fseek(fp, 0, SEEK_END);
        size_t fileSize = ftell(fp);
        fseek(fp, index, SEEK_SET);
        size_t length = fileSize - index - WORD_SIZE;

        uint8_t *data = malloc(length * sizeof *data);
        CHECK_ALLOC(data, "data");

        fread(data, sizeof *data, length, fp);

        parse_image_data(imageData, im, data, length);

        free(data);
    }

    fclose(fp);
    free_jpeg(imageData);

    return im;
}