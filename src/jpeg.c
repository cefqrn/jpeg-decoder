#include "bitstream.h"
#include "decoding.h"
#include "hufftree.h"
#include "parsing.h"
#include "macros.h"
#include "image.h"
#include "jpeg.h"

#include <limits.h>
#include <stddef.h>
#include <math.h>

#define CHAR_WIDTH  8
#define MARKER_SIZE 2

#define READ_WORD(fp) (getc(fp) << CHAR_WIDTH | getc(fp))

/*
    Read the data given by the jpeg segments into info.
    jpeg_free must be called to free the memory allocated in info.
    Returns 0 on success and -1 on failure.
*/
int jpeg_read_info(jpeg_info *jpegInfo, scan_info *scanInfo, FILE *fp) {
    // File must start with FFD8
    if (READ_WORD(fp) != SOI)
        return -1;

    if (feof(fp) || ferror(fp))
        return -1;

    SegmentMarker marker;
    do {
        marker = READ_WORD(fp);

        unsigned short segmentLength = READ_WORD(fp);
        if (segmentLength < 2)
            return -1;

        unsigned short dataLength = segmentLength - MARKER_SIZE;
        unsigned char data[USHRT_MAX];
        if (fread(data, sizeof *data, dataLength, fp) != dataLength)
            return -1;

        int errorCode = 0;
        switch (marker) {
            case SOF0: errorCode = parse_SOF0(jpegInfo, dataLength, data);          break;
            case DHT:  errorCode = parse_DHT(jpegInfo, dataLength, data);           break;
            case DQT:  errorCode = parse_DQT(jpegInfo, dataLength, data);           break;
            case SOS:  errorCode = parse_SOS(scanInfo, jpegInfo, dataLength, data); break;
            default:;  // All other markers are ignored.
        }

        if (errorCode)
            return errorCode;
    } while (marker != SOS);

    return 0;
}

/*
    Read the pixel values of the image into img.
*/
void jpeg_read_image(pixel *img, const jpeg_info *jpegInfo, const scan_info *scanInfo, FILE *fp) {
    bitstream stream = bitstream_create(fp);

    unsigned MCUWidth = 8 * scanInfo->maxHSamplingFactor;
    unsigned MCUHeight = 8 * scanInfo->maxVSamplingFactor;

    unsigned componentCount = scanInfo->componentCount;
    unsigned short imageWidth = jpegInfo->width;
    unsigned short imageHeight = jpegInfo->height;

    int dcCoefficients[4] = {0};
    for (unsigned y=0; y < imageHeight; y += MCUHeight) {
        for (unsigned x=0; x < imageWidth; x += MCUWidth) {
            for (unsigned i=0; i < componentCount; ++i) {
                unsigned HSamplingFactor = jpegInfo->componentInfo[scanInfo->componentInfo[i].componentID].HSamplingFactor;
                unsigned VSamplingFactor = jpegInfo->componentInfo[scanInfo->componentInfo[i].componentID].VSamplingFactor;

                for (unsigned v=0; v < VSamplingFactor; ++v) {
                    for (unsigned h=0; h < HSamplingFactor; ++h) {
                        decode_data_unit(img, jpegInfo, scanInfo->componentInfo[i], &stream, dcCoefficients + i, x + 8*h, y + 8*v, scanInfo->maxHSamplingFactor / HSamplingFactor, scanInfo->maxVSamplingFactor / VSamplingFactor);
                    }
                }
            }
        }
    }
}

/*
    Free the memory taken by info.
*/
void jpeg_free(jpeg_info *info) {
    for (size_t i=0; i < 2; ++i) {
        for (size_t j=0; j < 2; ++j) {
            hufftree_destroy(&info->huffmanTables[i][j]);
        }
    }
}
