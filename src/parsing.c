#include "hufftree.h"
#include "parsing.h"
#include "macros.h"
#include "jpeg.h"

#include <stddef.h>
#include <string.h>

#define CHAR_WIDTH 8

#define GET_WORD(data, index) (data[index] << CHAR_WIDTH) + data[index + 1]

void parse_APP0(jpeg_info *info, const unsigned char *data) {
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

void parse_SOF0(jpeg_info *info, const unsigned char *data) {
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

void parse_DHT(jpeg_info *info, const unsigned char *data, size_t length) {
    size_t offset = 0;

    do {
        size_t class = (data[offset] & 0x10) >> 4;
        size_t id    =  data[offset] & 0x1;

        ++offset;

        offset += hufftree_parse(&info->huffTrees[id][class], data + offset);
    } while (offset < length);
}

void parse_DQT(jpeg_info *info, const unsigned char *data, size_t length) {
    size_t offset = 0;
    
    do {
        size_t id = data[offset++] & 0x1;
        for (size_t i=0; i < 64; ++i)
            info->quantTables[id][i] = data[offset++];
    } while (offset < length);
}

void parse_SOS(jpeg_info *info, const unsigned char *data) {
    unsigned componentCount = data[0];

    for (size_t i=0; i < componentCount; ++i)
        info->componentData[i].hTreeId = data[2 + i * 2] >> 4;
}
