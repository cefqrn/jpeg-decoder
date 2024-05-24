#include "hufftree.h"
#include "parsing.h"
#include "macros.h"
#include "jpeg.h"

#include <stddef.h>
#include <string.h>

#define CHAR_WIDTH 8

#define GET_WORD(data, index) ((data[index] << CHAR_WIDTH) + data[index + 1])

int parse_SOF0(jpeg_info *info, unsigned short length, const unsigned char *data) {
    if (length < 6)
        return -1;

    unsigned componentCount = data[5];
    unsigned expectedLength = 6 + componentCount*3;
    if (length != expectedLength)
        return -1;

    // sample precision of 12 is not supported yet
    info->precision = data[0];
    if (info->precision != 8)
        return -1;

    info->height = GET_WORD(data, 1);
    // if height is 0, it's defined by the DNL segment marker (not supported yet)
    if (info->height < 1)
        return -1;

    info->width = GET_WORD(data, 3);
    if (info->width < 1)
        return -1;

    for (unsigned i=0; i < componentCount; ++i) {
        unsigned componentID         = data[6 + i*3];
        unsigned HSamplingFactor     = data[7 + i*3] >> 4;
        unsigned VSamplingFactor     = data[7 + i*3] & 0x0f;
        unsigned quantizationTableID = data[8 + i*3];

        if (HSamplingFactor == 0 || HSamplingFactor > 4)
            return -1;
        
        if (VSamplingFactor == 0 || VSamplingFactor > 4)
            return -1;

        if (quantizationTableID >= 4)
            return -1;

        info->componentInfo[componentID] = (struct jpeg_component_info){
            .HSamplingFactor     = HSamplingFactor,
            .VSamplingFactor     = VSamplingFactor,
            .quantizationTableID = quantizationTableID
        };
    }

    return 0;
}

int parse_DHT(jpeg_info *info, unsigned short length, const unsigned char *data) {
    if (length < 1)
        return -1;

    size_t offset = 0;
    do {
        unsigned class = data[offset] >> 4;
        unsigned id    = data[offset] & 0x0f;
        ++offset;

        if (class >= 2 || id >= 4)
            return -1;

        offset += hufftree_parse(&info->huffmanTables[class][id], data + offset);
    } while (offset < length);

    return 0;
}

int parse_DQT(jpeg_info *info, unsigned short length, const unsigned char *data) {
    if (!length || length % (64 + 1) != 0)
        return -1;

    size_t offset = 0;
    do {
        unsigned precision = data[offset] >> 4;
        unsigned id        = data[offset] & 0x0f;
        ++offset;

        // a quantization table precision of 16 bits is only available
        // if the sample precision is 12 bits (not supported)
        if (precision >= 1)
            return -1;

        if (id >= 4)
            return -1;

        for (unsigned i=0; i < 64; ++i)
            info->quantizationTables[id][i] = data[offset++];
    } while (length -= 64 + 1);

    return 0;
}

static inline unsigned max(unsigned a, unsigned b) {
    return a > b ? a : b;
}

int parse_SOS(scan_info *scanInfo, const jpeg_info *jpegInfo, unsigned short length, const unsigned char *data) {
    unsigned componentCount = data[0];
    unsigned expectedLength = 1 + componentCount*2 + 3;
    if (length != expectedLength)
        return -1;

    scanInfo->componentCount = componentCount;

    unsigned maxHSamplingFactor = 0;
    unsigned maxVSamplingFactor = 0;
    for (unsigned i=0; i < componentCount; ++i) {
        unsigned componentID = data[1 + i*2];

        unsigned DCHuffmanTableID = data[2 + i*2] >> 4;
        unsigned ACHuffmanTableID = data[2 + i*2] & 0x0f;

        if (DCHuffmanTableID >= 4 || ACHuffmanTableID >= 4)
            return -1;

        scanInfo->componentInfo[i] = (struct scan_component_info){
            .componentID = componentID,
            .DCHuffmanTableID = DCHuffmanTableID,
            .ACHuffmanTableID = ACHuffmanTableID
        };

        maxHSamplingFactor = max(maxHSamplingFactor, jpegInfo->componentInfo[componentID].HSamplingFactor);
        maxVSamplingFactor = max(maxVSamplingFactor, jpegInfo->componentInfo[componentID].VSamplingFactor);
    }

    scanInfo->maxHSamplingFactor = maxHSamplingFactor;
    scanInfo->maxVSamplingFactor = maxVSamplingFactor;

    return 0;
}
