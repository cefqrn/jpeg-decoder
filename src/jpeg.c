#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define CHAR_SIZE 8
#define WORD_SIZE 2 * CHAR_SIZE
#define READ_WORD(fp) getc(fp) << CHAR_SIZE | getc(fp)

typedef enum SegmentMarker {
    SOI = 0xFFD8,
    APP0 = 0xFFE0,
    SOF = 0xFFC0,
    DHT = 0xFFC4,
    DQT = 0xFFDB,
    SOS = 0xFFDA,
} SegmentMarker;

typedef struct image {
    uint16_t width;
    uint16_t height;
    uint8_t (**pixels)[3]; // RGB
} image;

typedef struct component_data {
    uint8_t id;
    uint8_t vSamplingFactor:4;
    uint8_t hSamplingFactor:4;
    uint8_t qTableId;
} component_data;

typedef struct jpeg_data {
    uint16_t width;
    uint16_t height;
    uint8_t (**pixels)[3]; // YCbCr
    uint8_t precision;
    uint8_t componentCount;
    component_data **componentData;
} jpeg_data;

static int parse_APP0(jpeg_data *imageData, uint8_t *data, size_t length) {
    // not implemented
}

static int parse_SOF(jpeg_data *imageData, uint8_t *data, size_t length) {
    imageData->precision = data[0];
    imageData->height = data[1] << CHAR_SIZE + data[2];
    imageData->width = data[3] << CHAR_SIZE + data[4];
    imageData->componentCount = data[5];

    imageData->componentData = malloc(imageData->componentCount * sizeof *imageData->componentData);
    for (int i=0; i < imageData->componentCount; ++i) {
        component_data *componentData = malloc(sizeof *componentData);

        componentData->id = data[6 + i*3];
        componentData->vSamplingFactor = data[7 + i*3] >> 4;
        componentData->hSamplingFactor = data[7 + i*3] & 0xF;
        componentData->qTableId = data[8 + i*3] & 0xF;

        imageData->componentData[i] = componentData;
    }

    return 0;
}

static int parse_DHT(jpeg_data *imageData, uint8_t *data, size_t length) {
    // not implemented
}

static int parse_DQT(jpeg_data *imageData, uint8_t *data, size_t length) {
    // not implemented
}

static int parse_SOS(jpeg_data *imageData, FILE *fp) {
    // not implemented
}

static int parse_segment(jpeg_data *imageData, FILE *fp) {
    uint16_t marker = READ_WORD(fp);

    // segments that don't indicate their length
    switch (marker) {
        case SOI: return 0;
        case SOS: return parse_SOS(imageData, fp);
    }

    size_t length = READ_WORD(fp) - WORD_SIZE;
    uint8_t *data = malloc(length * sizeof *data);
    fread(data, length, sizeof(uint8_t), fp);

    switch (marker) {
        case APP0: return parse_APP0(imageData, data, length);
        case SOF: return parse_SOF(imageData, data, length);
        case DHT: return parse_DHT(imageData, data, length);
        case DQT: return parse_DQT(imageData, data, length);
    }

    printf("Could not identify marker: %04X", marker);
    return -1;
}

static image *jpeg_to_image(jpeg_data *data) {
    // not implemented
}

static void free_jpeg(jpeg_data *data) {
    for (int i=0; i < data->componentCount; ++i) {
        free(data->componentData[i]);
    }
    free(data->componentData);
    free(data);
}

image *jpg_fparse(char *path) {
    FILE *fp = fopen(path, "r");

    if (fp == NULL) {
        puts("Could not open file.");
        return NULL;
    }

    jpeg_data *imageData = calloc(1, sizeof *imageData);

    int exit_code;
    while ((exit_code = jpg_parse_segment(imageData, fp)) == 0);

    fclose(fp);

    if (exit_code != 1) {
        puts("Could not parse jpeg.");
        return NULL;
    }

    image *im = jpeg_to_image(imageData);
    
    free_jpeg(imageData);

    return jpeg_to_image(imageData);
}

void free_image(image *im) {
    for (size_t i=0; i < im->width; ++i) {
        free(im->pixels[i]);
    }
    free(im->pixels);
    free(im);
}